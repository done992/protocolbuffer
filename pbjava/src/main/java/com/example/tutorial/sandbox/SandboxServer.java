package com.example.tutorial.sandbox;

import io.netty.bootstrap.ServerBootstrap;
import io.netty.channel.ChannelOption;
import io.netty.channel.EventLoopGroup;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.nio.NioServerSocketChannel;

import java.util.List;
import java.util.concurrent.Executors;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.example.tutorial.sandbox.SandboxProtocol.SandboxProtocolService;
import com.example.tutorial.sandbox.SandboxServiceFactory.BlockingSandboxService;
import com.google.protobuf.BlockingService;
import com.google.protobuf.ExtensionRegistry;
import com.google.protobuf.Service;
import com.googlecode.protobuf.pro.duplex.CleanShutdownHandler;
import com.googlecode.protobuf.pro.duplex.PeerInfo;
import com.googlecode.protobuf.pro.duplex.RpcClientChannel;
import com.googlecode.protobuf.pro.duplex.RpcConnectionEventNotifier;
import com.googlecode.protobuf.pro.duplex.execute.RpcServerCallExecutor;
import com.googlecode.protobuf.pro.duplex.execute.ThreadPoolCallExecutor;
import com.googlecode.protobuf.pro.duplex.listener.RpcConnectionEventListener;
import com.googlecode.protobuf.pro.duplex.logging.CategoryPerServiceLogger;
import com.googlecode.protobuf.pro.duplex.server.DuplexTcpServerPipelineFactory;
import com.googlecode.protobuf.pro.duplex.util.RenamingThreadFactoryProxy;

public class SandboxServer {
  private static Logger log = LoggerFactory.getLogger(SandboxServer.class); 

  public static void main(String[] args) throws InterruptedException {
    PeerInfo serverInfo = new PeerInfo(Constants.ServerHostname, Integer.parseInt(Constants.ServerPort));
    log.info("server peer initiated with [" + Constants.ServerHostname + "," + Constants.ServerPort + "]");
    
    // RPC payloads are uncompressed when logged - so reduce logging
    CategoryPerServiceLogger logger = new CategoryPerServiceLogger();
    logger.setLogRequestProto(false);
    logger.setLogResponseProto(false);
    
    // Configure the server.
    DuplexTcpServerPipelineFactory serverFactory = new DuplexTcpServerPipelineFactory(serverInfo);
    
    ExtensionRegistry r = ExtensionRegistry.newInstance();
    SandboxProtocol.registerAllExtensions(r);
    serverFactory.setExtensionRegistry(r);
    
    RpcServerCallExecutor rpcExecutor = new ThreadPoolCallExecutor(10, 10);
    serverFactory.setRpcServerCallExecutor(rpcExecutor);
    serverFactory.setLogger(logger);
    
    //setup a RPC event listener - it just logs what happens
    RpcConnectionEventNotifier rpcEventNotifier = new RpcConnectionEventNotifier();
    RpcConnectionEventListener listener = new RpcConnectionEventListener() {
      @Override
      public void connectionLost(RpcClientChannel clientChannel) {
        log.info("connectionLost" + clientChannel);
      }
      @Override
      public void connectionOpened(RpcClientChannel clientChannel) {
        log.info("connectionOpened" + clientChannel);

      }
      @Override
      public void connectionReestablished(RpcClientChannel clientChannel) {
        log.info("connectionReestablished" + clientChannel);
      }
      @Override
      public void connectionChanged(RpcClientChannel clientChannel) {
        log.info("connectionChanged" + clientChannel);
      }
    };
    rpcEventNotifier.setEventListener(listener);
    serverFactory.registerConnectionEventListener(rpcEventNotifier);
    
    //a blocking sandbox service
    BlockingService bSandboxService = SandboxProtocolService.newReflectiveBlockingService(new SandboxServiceFactory.BlockingSandboxService());
    serverFactory.getRpcServiceRegistry().registerService(true, bSandboxService);
        
    //a nb sandbox service
//    Service nbsandboxService = SandboxProtocolService
//        .newReflectiveService(new SandboxServiceFactory.NonBlockingSandboxService());
//    serverFactory.getRpcServiceRegistry().registerService(true, nbsandboxService);
    
    
    ServerBootstrap bootstrap = new ServerBootstrap();
    EventLoopGroup boss = new NioEventLoopGroup(2,new RenamingThreadFactoryProxy("boss", Executors.defaultThreadFactory()));
    EventLoopGroup workers = new NioEventLoopGroup(2,new RenamingThreadFactoryProxy("worker", Executors.defaultThreadFactory()));
    bootstrap.group(boss,workers);
    
    bootstrap.channel(NioServerSocketChannel.class);
    bootstrap.option(ChannelOption.SO_SNDBUF, 1048576);
    bootstrap.option(ChannelOption.SO_RCVBUF, 1048576);
    bootstrap.childOption(ChannelOption.SO_RCVBUF, 1048576);
    bootstrap.childOption(ChannelOption.SO_SNDBUF, 1048576);
    bootstrap.option(ChannelOption.TCP_NODELAY, true);
    bootstrap.childHandler(serverFactory);
    bootstrap.localAddress(serverInfo.getPort());
    
    CleanShutdownHandler shutdownHandler = new CleanShutdownHandler();
    shutdownHandler.addResource(boss);
    shutdownHandler.addResource(workers);
    shutdownHandler.addResource(rpcExecutor);
    
    // Bind and start to accept incoming connections.
    bootstrap.bind();
    log.info("Service " + bootstrap);
    
    while (true) {
      List<RpcClientChannel> clients = serverFactory.getRpcClientRegistry().getAllClients();
      log.info("Number of clients = " + clients.size());
      Thread.sleep(5000);
    }
  }
}

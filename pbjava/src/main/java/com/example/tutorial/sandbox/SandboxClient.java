package com.example.tutorial.sandbox;

import io.netty.bootstrap.Bootstrap;
import io.netty.channel.ChannelOption;
import io.netty.channel.EventLoopGroup;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.nio.NioSocketChannel;

import java.io.IOException;
import java.util.concurrent.Executors;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.example.tutorial.sandbox.SandboxProtocol.RequestProto;
import com.example.tutorial.sandbox.SandboxProtocol.ResponseProto;
import com.example.tutorial.sandbox.SandboxProtocol.SandboxProtocolService;
import com.google.protobuf.ExtensionRegistry;
import com.google.protobuf.ServiceException;
import com.googlecode.protobuf.pro.duplex.CleanShutdownHandler;
import com.googlecode.protobuf.pro.duplex.ClientRpcController;
import com.googlecode.protobuf.pro.duplex.PeerInfo;
import com.googlecode.protobuf.pro.duplex.RpcClientChannel;
import com.googlecode.protobuf.pro.duplex.RpcConnectionEventNotifier;
import com.googlecode.protobuf.pro.duplex.client.DuplexTcpClientPipelineFactory;
import com.googlecode.protobuf.pro.duplex.client.RpcClientConnectionWatchdog;
import com.googlecode.protobuf.pro.duplex.execute.RpcServerCallExecutor;
import com.googlecode.protobuf.pro.duplex.execute.ThreadPoolCallExecutor;
import com.googlecode.protobuf.pro.duplex.listener.RpcConnectionEventListener;
import com.googlecode.protobuf.pro.duplex.logging.CategoryPerServiceLogger;
import com.googlecode.protobuf.pro.duplex.util.RenamingThreadFactoryProxy;


public class SandboxClient {
  private static Logger log = LoggerFactory.getLogger(SandboxClient.class);
  private static RpcClientChannel channel = null;
  private static final int requestvalue = 9;

  public static void main(String[] args) throws InterruptedException, IOException {
    
    PeerInfo client = new PeerInfo(Constants.ClientHostname, Integer.parseInt(Constants.ClientPort));
    PeerInfo server = new PeerInfo(Constants.ServerHostname, Integer.parseInt(Constants.ServerPort));
    
    DuplexTcpClientPipelineFactory clientFactory = new DuplexTcpClientPipelineFactory();
    //force the use of a local port. normally you don't need this.
    clientFactory.setClientInfo(client);
    
    ExtensionRegistry r = ExtensionRegistry.newInstance();
    SandboxProtocol.registerAllExtensions(r);
    clientFactory.setExtensionRegistry(r);
    
    clientFactory.setConnectResponseTimeoutMillis(10000);
    RpcServerCallExecutor rpcExecutor = new ThreadPoolCallExecutor(3, 10);
    clientFactory.setRpcServerCallExecutor(rpcExecutor);
    
    //RPC payloads are uncompressed when logged - so reduce logging
    CategoryPerServiceLogger logger = new CategoryPerServiceLogger();
    logger.setLogRequestProto(false);
    logger.setLogResponseProto(false);
    clientFactory.setRpcLogger(logger);
    
    //Set up the event pipeline factory
    //setup a RPC event listener -  it just logs what happens
    RpcConnectionEventNotifier rpcEventNotifier = new RpcConnectionEventNotifier();
    final RpcConnectionEventListener listener = new RpcConnectionEventListener() {

      @Override
      public void connectionLost(RpcClientChannel clientChannel) {
        log.info("connectionLost" + clientChannel);
      }

      @Override
      public void connectionOpened(RpcClientChannel clientChannel) {
        log.info("connectionOpened" + clientChannel);
        channel = clientChannel;
      }

      @Override
      public void connectionReestablished(RpcClientChannel clientChannel) {
        log.info("connectionReestablished"  + clientChannel);
        channel = clientChannel;
      }

      @Override
      public void connectionChanged(RpcClientChannel clientChannel) {
        log.info("connectionChanged" + clientChannel);
        channel = clientChannel;
      }
    };
    rpcEventNotifier.addEventListener(listener);
    clientFactory.removeConnectionEventListener(rpcEventNotifier);
    
    Bootstrap bootstrap = new Bootstrap();
    EventLoopGroup workers = new NioEventLoopGroup(16,
        new RenamingThreadFactoryProxy("workers",
            Executors.defaultThreadFactory()) );
    
    bootstrap.group(workers);
    bootstrap.handler(clientFactory);
    bootstrap.channel(NioSocketChannel.class);
    bootstrap.option(ChannelOption.TCP_NODELAY, true);
    bootstrap.option(ChannelOption.CONNECT_TIMEOUT_MILLIS, 10000);
    bootstrap.option(ChannelOption.SO_SNDBUF, 1048576);
    bootstrap.option(ChannelOption.SO_RCVBUF, 1048576);
    
    RpcClientConnectionWatchdog watchdog = new RpcClientConnectionWatchdog(clientFactory, bootstrap);
    rpcEventNotifier.addEventListener(watchdog);
    watchdog.start();
    
    
    CleanShutdownHandler shutdownHandler = new CleanShutdownHandler();
    shutdownHandler.addResource(workers);
    shutdownHandler.addResource(rpcExecutor);
    
    clientFactory.peerWith(server, bootstrap);
    
    System.out.println(" 1111 " );
    log.error(" 1 ");
    
    if (channel == null) {
      log.error(" null ");
    }
    
    while (true && channel != null) {
      SandboxProtocolService.BlockingInterface blockingService = SandboxProtocolService.newBlockingStub(channel);
      final ClientRpcController controller = channel.newRpcController();
      controller.setTimeoutMs(0);
      
      RequestProto.Builder requestBuilder = RequestProto.newBuilder();
      requestBuilder.setValue(requestvalue);
      RequestProto requestProto = requestBuilder.build();
      
      try {
        ResponseProto responseProto = blockingService.ping(controller, requestProto);
        log.error("====================================");
        if (responseProto.hasValue()) {
          System.out.println(" response value = " + responseProto.getValue());
          log.error(" response value = " + responseProto.getValue());
        }
      } catch (ServiceException e) {
        log.warn("call failed.", e);
      }
      Thread.sleep(10000);
    }
    
    
  }
}

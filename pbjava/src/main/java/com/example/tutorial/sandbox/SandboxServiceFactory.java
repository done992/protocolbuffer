package com.example.tutorial.sandbox;


import com.example.tutorial.sandbox.SandboxProtocol.RequestProto;
import com.example.tutorial.sandbox.SandboxProtocol.ResponseProto;
import com.example.tutorial.sandbox.SandboxProtocol.SandboxProtocolService;
import com.google.protobuf.RpcCallback;
import com.google.protobuf.RpcController;
import com.google.protobuf.ServiceException;

public class SandboxServiceFactory {
  public static class BlockingSandboxService implements SandboxProtocolService.BlockingInterface  {
                                                              
    @Override
    public ResponseProto ping(RpcController controller, RequestProto request)
        throws ServiceException {
      if (controller.isCanceled()) {
        return null;
      }
      
      int value = -1;
      if (request.hasValue()) {
        value = request.getValue();
      }
      
      ResponseProto.Builder  builder = ResponseProto.newBuilder();
      builder.setValue(value+1);
      ResponseProto responseProto = builder.build();
      
      return responseProto;
    }
  }
  
  public static class NonBlockingSandboxService implements SandboxProtocolService.Interface {
    @Override
    public void ping(RpcController controller, RequestProto request,
        RpcCallback<ResponseProto> done) {
      if (controller.isCanceled()) {
        return;
      }
      
      int value = -1;
      if (request.hasValue()) {
        value = request.getValue();
      }
      
      ResponseProto.Builder  builder = ResponseProto.newBuilder();
      builder.setValue(value+1);
      ResponseProto responseProto = builder.build();
      
      done.run(responseProto);
    }
  }

}

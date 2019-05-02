/**
 * @fileoverview gRPC-Web generated client stub for 
 * @enhanceable
 * @public
 */

// GENERATED CODE -- DO NOT EDIT!


import * as grpcWeb from 'grpc-web';

import {
  NewSessionRequest,
  NewSessionResponse} from './service_pb';

export class UIServiceClient {
  client_: grpcWeb.AbstractClientBase;
  hostname_: string;
  credentials_: null | { [index: string]: string; };
  options_: null | { [index: string]: string; };

  constructor (hostname: string,
               credentials: null | { [index: string]: string; },
               options: null | { [index: string]: string; }) {
    if (!options) options = {};
    options['format'] = 'text';

    this.client_ = new grpcWeb.GrpcWebClientBase(options);
    this.hostname_ = hostname;
    this.credentials_ = credentials;
    this.options_ = options;
  }

  methodInfoNewSession = new grpcWeb.AbstractClientBase.MethodInfo(
    NewSessionResponse,
    (request: NewSessionRequest) => {
      return request.serializeBinary();
    },
    NewSessionResponse.deserializeBinary
  );

  newSession(
    request: NewSessionRequest,
    metadata: grpcWeb.Metadata | null,
    callback: (err: grpcWeb.Error,
               response: NewSessionResponse) => void) {
    return this.client_.rpcCall(
      this.hostname_ +
        '/UIService/NewSession',
      request,
      metadata || {},
      this.methodInfoNewSession,
      callback);
  }

}


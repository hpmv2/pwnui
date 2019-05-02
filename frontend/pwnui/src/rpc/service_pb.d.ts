import * as jspb from "google-protobuf"

export class NewSessionRequest extends jspb.Message {
  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): NewSessionRequest.AsObject;
  static toObject(includeInstance: boolean, msg: NewSessionRequest): NewSessionRequest.AsObject;
  static serializeBinaryToWriter(message: NewSessionRequest, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): NewSessionRequest;
  static deserializeBinaryFromReader(message: NewSessionRequest, reader: jspb.BinaryReader): NewSessionRequest;
}

export namespace NewSessionRequest {
  export type AsObject = {
  }
}

export class NewSessionResponse extends jspb.Message {
  getId(): number;
  setId(value: number): void;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): NewSessionResponse.AsObject;
  static toObject(includeInstance: boolean, msg: NewSessionResponse): NewSessionResponse.AsObject;
  static serializeBinaryToWriter(message: NewSessionResponse, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): NewSessionResponse;
  static deserializeBinaryFromReader(message: NewSessionResponse, reader: jspb.BinaryReader): NewSessionResponse;
}

export namespace NewSessionResponse {
  export type AsObject = {
    id: number,
  }
}


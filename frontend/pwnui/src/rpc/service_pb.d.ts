import * as jspb from "google-protobuf"

export class NewSessionRequest extends jspb.Message {
  getBinary(): string;
  setBinary(value: string): void;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): NewSessionRequest.AsObject;
  static toObject(includeInstance: boolean, msg: NewSessionRequest): NewSessionRequest.AsObject;
  static serializeBinaryToWriter(message: NewSessionRequest, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): NewSessionRequest;
  static deserializeBinaryFromReader(message: NewSessionRequest, reader: jspb.BinaryReader): NewSessionRequest;
}

export namespace NewSessionRequest {
  export type AsObject = {
    binary: string,
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

export class IOServerRequest extends jspb.Message {
  getStartId(): number;
  setStartId(value: number): void;
  hasStartId(): boolean;

  getRead(): IOReadRequest | undefined;
  setRead(value?: IOReadRequest): void;
  hasRead(): boolean;
  clearRead(): void;
  hasRead(): boolean;

  getWrite(): IOWriteRequest | undefined;
  setWrite(value?: IOWriteRequest): void;
  hasWrite(): boolean;
  clearWrite(): void;
  hasWrite(): boolean;

  getRequestCase(): IOServerRequest.RequestCase;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): IOServerRequest.AsObject;
  static toObject(includeInstance: boolean, msg: IOServerRequest): IOServerRequest.AsObject;
  static serializeBinaryToWriter(message: IOServerRequest, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): IOServerRequest;
  static deserializeBinaryFromReader(message: IOServerRequest, reader: jspb.BinaryReader): IOServerRequest;
}

export namespace IOServerRequest {
  export type AsObject = {
    startId: number,
    read?: IOReadRequest.AsObject,
    write?: IOWriteRequest.AsObject,
  }

  export enum RequestCase { 
    REQUEST_NOT_SET = 0,
    START_ID = 1,
    READ = 2,
    WRITE = 3,
  }
}

export class IOServerResponse extends jspb.Message {
  getAck(): boolean;
  setAck(value: boolean): void;
  hasAck(): boolean;

  getRead(): IOReadResponse | undefined;
  setRead(value?: IOReadResponse): void;
  hasRead(): boolean;
  clearRead(): void;
  hasRead(): boolean;

  getResponseCase(): IOServerResponse.ResponseCase;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): IOServerResponse.AsObject;
  static toObject(includeInstance: boolean, msg: IOServerResponse): IOServerResponse.AsObject;
  static serializeBinaryToWriter(message: IOServerResponse, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): IOServerResponse;
  static deserializeBinaryFromReader(message: IOServerResponse, reader: jspb.BinaryReader): IOServerResponse;
}

export namespace IOServerResponse {
  export type AsObject = {
    ack: boolean,
    read?: IOReadResponse.AsObject,
  }

  export enum ResponseCase { 
    RESPONSE_NOT_SET = 0,
    ACK = 1,
    READ = 2,
  }
}

export class IOReadRequest extends jspb.Message {
  getChunksList(): Array<IOReadChunk>;
  setChunksList(value: Array<IOReadChunk>): void;
  clearChunksList(): void;
  addChunks(value?: IOReadChunk, index?: number): IOReadChunk;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): IOReadRequest.AsObject;
  static toObject(includeInstance: boolean, msg: IOReadRequest): IOReadRequest.AsObject;
  static serializeBinaryToWriter(message: IOReadRequest, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): IOReadRequest;
  static deserializeBinaryFromReader(message: IOReadRequest, reader: jspb.BinaryReader): IOReadRequest;
}

export namespace IOReadRequest {
  export type AsObject = {
    chunksList: Array<IOReadChunk.AsObject>,
  }
}

export class IOReadChunk extends jspb.Message {
  getRegex(): IOReadChunk.Regex | undefined;
  setRegex(value?: IOReadChunk.Regex): void;
  hasRegex(): boolean;
  clearRegex(): void;
  hasRegex(): boolean;

  getLittleEndian32(): boolean;
  setLittleEndian32(value: boolean): void;
  hasLittleEndian32(): boolean;

  getReadCase(): IOReadChunk.ReadCase;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): IOReadChunk.AsObject;
  static toObject(includeInstance: boolean, msg: IOReadChunk): IOReadChunk.AsObject;
  static serializeBinaryToWriter(message: IOReadChunk, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): IOReadChunk;
  static deserializeBinaryFromReader(message: IOReadChunk, reader: jspb.BinaryReader): IOReadChunk;
}

export namespace IOReadChunk {
  export type AsObject = {
    regex?: IOReadChunk.Regex.AsObject,
    littleEndian32: boolean,
  }

  export class Regex extends jspb.Message {
    getRegex(): string;
    setRegex(value: string): void;

    getGroupsList(): Array<IOElementType>;
    setGroupsList(value: Array<IOElementType>): void;
    clearGroupsList(): void;
    addGroups(value: IOElementType, index?: number): void;

    serializeBinary(): Uint8Array;
    toObject(includeInstance?: boolean): Regex.AsObject;
    static toObject(includeInstance: boolean, msg: Regex): Regex.AsObject;
    static serializeBinaryToWriter(message: Regex, writer: jspb.BinaryWriter): void;
    static deserializeBinary(bytes: Uint8Array): Regex;
    static deserializeBinaryFromReader(message: Regex, reader: jspb.BinaryReader): Regex;
  }

  export namespace Regex {
    export type AsObject = {
      regex: string,
      groupsList: Array<IOElementType>,
    }
  }


  export enum ReadCase { 
    READ_NOT_SET = 0,
    REGEX = 1,
    LITTLE_ENDIAN_32 = 2,
  }
}

export class IOReadResponse extends jspb.Message {
  getChunksList(): Array<IOReadChunkResult>;
  setChunksList(value: Array<IOReadChunkResult>): void;
  clearChunksList(): void;
  addChunks(value?: IOReadChunkResult, index?: number): IOReadChunkResult;

  getError(): string;
  setError(value: string): void;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): IOReadResponse.AsObject;
  static toObject(includeInstance: boolean, msg: IOReadResponse): IOReadResponse.AsObject;
  static serializeBinaryToWriter(message: IOReadResponse, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): IOReadResponse;
  static deserializeBinaryFromReader(message: IOReadResponse, reader: jspb.BinaryReader): IOReadResponse;
}

export namespace IOReadResponse {
  export type AsObject = {
    chunksList: Array<IOReadChunkResult.AsObject>,
    error: string,
  }
}

export class IOReadChunkResult extends jspb.Message {
  getData(): Uint8Array | string;
  getData_asU8(): Uint8Array;
  getData_asB64(): string;
  setData(value: Uint8Array | string): void;

  getGroupsList(): Array<string>;
  setGroupsList(value: Array<string>): void;
  clearGroupsList(): void;
  addGroups(value: string, index?: number): void;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): IOReadChunkResult.AsObject;
  static toObject(includeInstance: boolean, msg: IOReadChunkResult): IOReadChunkResult.AsObject;
  static serializeBinaryToWriter(message: IOReadChunkResult, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): IOReadChunkResult;
  static deserializeBinaryFromReader(message: IOReadChunkResult, reader: jspb.BinaryReader): IOReadChunkResult;
}

export namespace IOReadChunkResult {
  export type AsObject = {
    data: Uint8Array | string,
    groupsList: Array<string>,
  }
}

export class IOWriteRequest extends jspb.Message {
  getChunksList(): Array<IOWriteChunk>;
  setChunksList(value: Array<IOWriteChunk>): void;
  clearChunksList(): void;
  addChunks(value?: IOWriteChunk, index?: number): IOWriteChunk;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): IOWriteRequest.AsObject;
  static toObject(includeInstance: boolean, msg: IOWriteRequest): IOWriteRequest.AsObject;
  static serializeBinaryToWriter(message: IOWriteRequest, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): IOWriteRequest;
  static deserializeBinaryFromReader(message: IOWriteRequest, reader: jspb.BinaryReader): IOWriteRequest;
}

export namespace IOWriteRequest {
  export type AsObject = {
    chunksList: Array<IOWriteChunk.AsObject>,
  }
}

export class IOWriteChunk extends jspb.Message {
  getData(): Uint8Array | string;
  getData_asU8(): Uint8Array;
  getData_asB64(): string;
  setData(value: Uint8Array | string): void;
  hasData(): boolean;

  getDecimalInteger(): number;
  setDecimalInteger(value: number): void;
  hasDecimalInteger(): boolean;

  getLittleEndianInteger(): IOWriteChunk.LittleEndianInteger | undefined;
  setLittleEndianInteger(value?: IOWriteChunk.LittleEndianInteger): void;
  hasLittleEndianInteger(): boolean;
  clearLittleEndianInteger(): void;
  hasLittleEndianInteger(): boolean;

  getWriteCase(): IOWriteChunk.WriteCase;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): IOWriteChunk.AsObject;
  static toObject(includeInstance: boolean, msg: IOWriteChunk): IOWriteChunk.AsObject;
  static serializeBinaryToWriter(message: IOWriteChunk, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): IOWriteChunk;
  static deserializeBinaryFromReader(message: IOWriteChunk, reader: jspb.BinaryReader): IOWriteChunk;
}

export namespace IOWriteChunk {
  export type AsObject = {
    data: Uint8Array | string,
    decimalInteger: number,
    littleEndianInteger?: IOWriteChunk.LittleEndianInteger.AsObject,
  }

  export class LittleEndianInteger extends jspb.Message {
    getInteger(): number;
    setInteger(value: number): void;

    getWidth(): number;
    setWidth(value: number): void;

    serializeBinary(): Uint8Array;
    toObject(includeInstance?: boolean): LittleEndianInteger.AsObject;
    static toObject(includeInstance: boolean, msg: LittleEndianInteger): LittleEndianInteger.AsObject;
    static serializeBinaryToWriter(message: LittleEndianInteger, writer: jspb.BinaryWriter): void;
    static deserializeBinary(bytes: Uint8Array): LittleEndianInteger;
    static deserializeBinaryFromReader(message: LittleEndianInteger, reader: jspb.BinaryReader): LittleEndianInteger;
  }

  export namespace LittleEndianInteger {
    export type AsObject = {
      integer: number,
      width: number,
    }
  }


  export enum WriteCase { 
    WRITE_NOT_SET = 0,
    DATA = 1,
    DECIMAL_INTEGER = 2,
    LITTLE_ENDIAN_INTEGER = 3,
  }
}

export class UIIODataRequest extends jspb.Message {
  getSessionId(): number;
  setSessionId(value: number): void;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): UIIODataRequest.AsObject;
  static toObject(includeInstance: boolean, msg: UIIODataRequest): UIIODataRequest.AsObject;
  static serializeBinaryToWriter(message: UIIODataRequest, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): UIIODataRequest;
  static deserializeBinaryFromReader(message: UIIODataRequest, reader: jspb.BinaryReader): UIIODataRequest;
}

export namespace UIIODataRequest {
  export type AsObject = {
    sessionId: number,
  }
}

export class UIIODataUpdate extends jspb.Message {
  getAppendOperation(): UIIOOperation | undefined;
  setAppendOperation(value?: UIIOOperation): void;
  hasAppendOperation(): boolean;
  clearAppendOperation(): void;
  hasAppendOperation(): boolean;

  getUpdateChunk(): UIIOChunkUpdate | undefined;
  setUpdateChunk(value?: UIIOChunkUpdate): void;
  hasUpdateChunk(): boolean;
  clearUpdateChunk(): void;
  hasUpdateChunk(): boolean;

  getTimestamp(): number;
  setTimestamp(value: number): void;

  getUpdateCase(): UIIODataUpdate.UpdateCase;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): UIIODataUpdate.AsObject;
  static toObject(includeInstance: boolean, msg: UIIODataUpdate): UIIODataUpdate.AsObject;
  static serializeBinaryToWriter(message: UIIODataUpdate, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): UIIODataUpdate;
  static deserializeBinaryFromReader(message: UIIODataUpdate, reader: jspb.BinaryReader): UIIODataUpdate;
}

export namespace UIIODataUpdate {
  export type AsObject = {
    appendOperation?: UIIOOperation.AsObject,
    updateChunk?: UIIOChunkUpdate.AsObject,
    timestamp: number,
  }

  export enum UpdateCase { 
    UPDATE_NOT_SET = 0,
    APPEND_OPERATION = 1,
    UPDATE_CHUNK = 2,
  }
}

export class UIIOChunkUpdate extends jspb.Message {
  getChunkId(): number;
  setChunkId(value: number): void;

  getElementsList(): Array<UIIOElement>;
  setElementsList(value: Array<UIIOElement>): void;
  clearElementsList(): void;
  addElements(value?: UIIOElement, index?: number): UIIOElement;

  getRemove(): boolean;
  setRemove(value: boolean): void;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): UIIOChunkUpdate.AsObject;
  static toObject(includeInstance: boolean, msg: UIIOChunkUpdate): UIIOChunkUpdate.AsObject;
  static serializeBinaryToWriter(message: UIIOChunkUpdate, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): UIIOChunkUpdate;
  static deserializeBinaryFromReader(message: UIIOChunkUpdate, reader: jspb.BinaryReader): UIIOChunkUpdate;
}

export namespace UIIOChunkUpdate {
  export type AsObject = {
    chunkId: number,
    elementsList: Array<UIIOElement.AsObject>,
    remove: boolean,
  }
}

export class UIIOOperation extends jspb.Message {
  getOpId(): number;
  setOpId(value: number): void;

  getChunksList(): Array<UIIOChunk>;
  setChunksList(value: Array<UIIOChunk>): void;
  clearChunksList(): void;
  addChunks(value?: UIIOChunk, index?: number): UIIOChunk;

  getSource(): UIIOOperation.UIIOOperationSource;
  setSource(value: UIIOOperation.UIIOOperationSource): void;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): UIIOOperation.AsObject;
  static toObject(includeInstance: boolean, msg: UIIOOperation): UIIOOperation.AsObject;
  static serializeBinaryToWriter(message: UIIOOperation, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): UIIOOperation;
  static deserializeBinaryFromReader(message: UIIOOperation, reader: jspb.BinaryReader): UIIOOperation;
}

export namespace UIIOOperation {
  export type AsObject = {
    opId: number,
    chunksList: Array<UIIOChunk.AsObject>,
    source: UIIOOperation.UIIOOperationSource,
  }

  export enum UIIOOperationSource { 
    INVALID = 0,
    STDIN = 1,
    STDOUT = 2,
  }
}

export class UIIOChunk extends jspb.Message {
  getChunkId(): number;
  setChunkId(value: number): void;

  getElementsList(): Array<UIIOElement>;
  setElementsList(value: Array<UIIOElement>): void;
  clearElementsList(): void;
  addElements(value?: UIIOElement, index?: number): UIIOElement;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): UIIOChunk.AsObject;
  static toObject(includeInstance: boolean, msg: UIIOChunk): UIIOChunk.AsObject;
  static serializeBinaryToWriter(message: UIIOChunk, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): UIIOChunk;
  static deserializeBinaryFromReader(message: UIIOChunk, reader: jspb.BinaryReader): UIIOChunk;
}

export namespace UIIOChunk {
  export type AsObject = {
    chunkId: number,
    elementsList: Array<UIIOElement.AsObject>,
  }
}

export class UIIOElement extends jspb.Message {
  getData(): Uint8Array | string;
  getData_asU8(): Uint8Array;
  getData_asB64(): string;
  setData(value: Uint8Array | string): void;

  getType(): IOElementType;
  setType(value: IOElementType): void;

  getPending(): boolean;
  setPending(value: boolean): void;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): UIIOElement.AsObject;
  static toObject(includeInstance: boolean, msg: UIIOElement): UIIOElement.AsObject;
  static serializeBinaryToWriter(message: UIIOElement, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): UIIOElement;
  static deserializeBinaryFromReader(message: UIIOElement, reader: jspb.BinaryReader): UIIOElement;
}

export namespace UIIOElement {
  export type AsObject = {
    data: Uint8Array | string,
    type: IOElementType,
    pending: boolean,
  }
}

export enum IOElementType { 
  IOET_INVALID = 0,
  IOET_UNINTERESTING = 1,
  IOET_UNPROCESSED = 2,
  IOET_RAW = 3,
  IOET_HEX32 = 4,
  IOET_DECIMAL32 = 5,
}

import * as jspb from "google-protobuf"

export class NewSessionRequest extends jspb.Message {
  getBinary(): string;
  setBinary(value: string): void;

  getScript(): string;
  setScript(value: string): void;

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
    script: string,
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

  getRead(): IOConsumer | undefined;
  setRead(value?: IOConsumer): void;
  hasRead(): boolean;
  clearRead(): void;
  hasRead(): boolean;

  getWrite(): IOProducer | undefined;
  setWrite(value?: IOProducer): void;
  hasWrite(): boolean;
  clearWrite(): void;
  hasWrite(): boolean;

  getSync(): IOConsumerSync | undefined;
  setSync(value?: IOConsumerSync): void;
  hasSync(): boolean;
  clearSync(): void;
  hasSync(): boolean;

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
    read?: IOConsumer.AsObject,
    write?: IOProducer.AsObject,
    sync?: IOConsumerSync.AsObject,
  }

  export enum RequestCase { 
    REQUEST_NOT_SET = 0,
    START_ID = 1,
    READ = 2,
    WRITE = 3,
    SYNC = 4,
  }
}

export class IOConsumer extends jspb.Message {
  getChainId(): number;
  setChainId(value: number): void;

  getInterp(): IOConsumerInterpretation;
  setInterp(value: IOConsumerInterpretation): void;

  getAny(): IOConsumer.Any | undefined;
  setAny(value?: IOConsumer.Any): void;
  hasAny(): boolean;
  clearAny(): void;
  hasAny(): boolean;

  getLine(): IOConsumer.Line | undefined;
  setLine(value?: IOConsumer.Line): void;
  hasLine(): boolean;
  clearLine(): void;
  hasLine(): boolean;

  getRegex(): IOConsumer.Regex | undefined;
  setRegex(value?: IOConsumer.Regex): void;
  hasRegex(): boolean;
  clearRegex(): void;
  hasRegex(): boolean;

  getLiteral(): IOConsumer.Literal | undefined;
  setLiteral(value?: IOConsumer.Literal): void;
  hasLiteral(): boolean;
  clearLiteral(): void;
  hasLiteral(): boolean;

  getNchars(): IOConsumer.NChars | undefined;
  setNchars(value?: IOConsumer.NChars): void;
  hasNchars(): boolean;
  clearNchars(): void;
  hasNchars(): boolean;

  getNumber(): IOConsumer.Number | undefined;
  setNumber(value?: IOConsumer.Number): void;
  hasNumber(): boolean;
  clearNumber(): void;
  hasNumber(): boolean;

  getChain(): IOConsumer.Chain | undefined;
  setChain(value?: IOConsumer.Chain): void;
  hasChain(): boolean;
  clearChain(): void;
  hasChain(): boolean;

  getOneof(): IOConsumer.OneOf | undefined;
  setOneof(value?: IOConsumer.OneOf): void;
  hasOneof(): boolean;
  clearOneof(): void;
  hasOneof(): boolean;

  getConsumerCase(): IOConsumer.ConsumerCase;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): IOConsumer.AsObject;
  static toObject(includeInstance: boolean, msg: IOConsumer): IOConsumer.AsObject;
  static serializeBinaryToWriter(message: IOConsumer, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): IOConsumer;
  static deserializeBinaryFromReader(message: IOConsumer, reader: jspb.BinaryReader): IOConsumer;
}

export namespace IOConsumer {
  export type AsObject = {
    chainId: number,
    interp: IOConsumerInterpretation,
    any?: IOConsumer.Any.AsObject,
    line?: IOConsumer.Line.AsObject,
    regex?: IOConsumer.Regex.AsObject,
    literal?: IOConsumer.Literal.AsObject,
    nchars?: IOConsumer.NChars.AsObject,
    number?: IOConsumer.Number.AsObject,
    chain?: IOConsumer.Chain.AsObject,
    oneof?: IOConsumer.OneOf.AsObject,
  }

  export class Any extends jspb.Message {
    serializeBinary(): Uint8Array;
    toObject(includeInstance?: boolean): Any.AsObject;
    static toObject(includeInstance: boolean, msg: Any): Any.AsObject;
    static serializeBinaryToWriter(message: Any, writer: jspb.BinaryWriter): void;
    static deserializeBinary(bytes: Uint8Array): Any;
    static deserializeBinaryFromReader(message: Any, reader: jspb.BinaryReader): Any;
  }

  export namespace Any {
    export type AsObject = {
    }
  }


  export class Line extends jspb.Message {
    getPredicate(): IOConsumer | undefined;
    setPredicate(value?: IOConsumer): void;
    hasPredicate(): boolean;
    clearPredicate(): void;

    serializeBinary(): Uint8Array;
    toObject(includeInstance?: boolean): Line.AsObject;
    static toObject(includeInstance: boolean, msg: Line): Line.AsObject;
    static serializeBinaryToWriter(message: Line, writer: jspb.BinaryWriter): void;
    static deserializeBinary(bytes: Uint8Array): Line;
    static deserializeBinaryFromReader(message: Line, reader: jspb.BinaryReader): Line;
  }

  export namespace Line {
    export type AsObject = {
      predicate?: IOConsumer.AsObject,
    }
  }


  export class Regex extends jspb.Message {
    getRegex(): string;
    setRegex(value: string): void;

    getGroupInterpList(): Array<IOConsumerInterpretation>;
    setGroupInterpList(value: Array<IOConsumerInterpretation>): void;
    clearGroupInterpList(): void;
    addGroupInterp(value: IOConsumerInterpretation, index?: number): void;

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
      groupInterpList: Array<IOConsumerInterpretation>,
    }
  }


  export class Literal extends jspb.Message {
    getLiteral(): string;
    setLiteral(value: string): void;

    serializeBinary(): Uint8Array;
    toObject(includeInstance?: boolean): Literal.AsObject;
    static toObject(includeInstance: boolean, msg: Literal): Literal.AsObject;
    static serializeBinaryToWriter(message: Literal, writer: jspb.BinaryWriter): void;
    static deserializeBinary(bytes: Uint8Array): Literal;
    static deserializeBinaryFromReader(message: Literal, reader: jspb.BinaryReader): Literal;
  }

  export namespace Literal {
    export type AsObject = {
      literal: string,
    }
  }


  export class NChars extends jspb.Message {
    getMin(): number;
    setMin(value: number): void;

    getMax(): number;
    setMax(value: number): void;

    serializeBinary(): Uint8Array;
    toObject(includeInstance?: boolean): NChars.AsObject;
    static toObject(includeInstance: boolean, msg: NChars): NChars.AsObject;
    static serializeBinaryToWriter(message: NChars, writer: jspb.BinaryWriter): void;
    static deserializeBinary(bytes: Uint8Array): NChars;
    static deserializeBinaryFromReader(message: NChars, reader: jspb.BinaryReader): NChars;
  }

  export namespace NChars {
    export type AsObject = {
      min: number,
      max: number,
    }
  }


  export class Number extends jspb.Message {
    serializeBinary(): Uint8Array;
    toObject(includeInstance?: boolean): Number.AsObject;
    static toObject(includeInstance: boolean, msg: Number): Number.AsObject;
    static serializeBinaryToWriter(message: Number, writer: jspb.BinaryWriter): void;
    static deserializeBinary(bytes: Uint8Array): Number;
    static deserializeBinaryFromReader(message: Number, reader: jspb.BinaryReader): Number;
  }

  export namespace Number {
    export type AsObject = {
    }
  }


  export class Chain extends jspb.Message {
    getConsumersList(): Array<IOConsumer>;
    setConsumersList(value: Array<IOConsumer>): void;
    clearConsumersList(): void;
    addConsumers(value?: IOConsumer, index?: number): IOConsumer;

    serializeBinary(): Uint8Array;
    toObject(includeInstance?: boolean): Chain.AsObject;
    static toObject(includeInstance: boolean, msg: Chain): Chain.AsObject;
    static serializeBinaryToWriter(message: Chain, writer: jspb.BinaryWriter): void;
    static deserializeBinary(bytes: Uint8Array): Chain;
    static deserializeBinaryFromReader(message: Chain, reader: jspb.BinaryReader): Chain;
  }

  export namespace Chain {
    export type AsObject = {
      consumersList: Array<IOConsumer.AsObject>,
    }
  }


  export class OneOf extends jspb.Message {
    getConsumersList(): Array<IOConsumer>;
    setConsumersList(value: Array<IOConsumer>): void;
    clearConsumersList(): void;
    addConsumers(value?: IOConsumer, index?: number): IOConsumer;

    serializeBinary(): Uint8Array;
    toObject(includeInstance?: boolean): OneOf.AsObject;
    static toObject(includeInstance: boolean, msg: OneOf): OneOf.AsObject;
    static serializeBinaryToWriter(message: OneOf, writer: jspb.BinaryWriter): void;
    static deserializeBinary(bytes: Uint8Array): OneOf;
    static deserializeBinaryFromReader(message: OneOf, reader: jspb.BinaryReader): OneOf;
  }

  export namespace OneOf {
    export type AsObject = {
      consumersList: Array<IOConsumer.AsObject>,
    }
  }


  export enum ConsumerCase { 
    CONSUMER_NOT_SET = 0,
    ANY = 3,
    LINE = 4,
    REGEX = 5,
    LITERAL = 6,
    NCHARS = 7,
    NUMBER = 8,
    CHAIN = 9,
    ONEOF = 10,
  }
}

export class IOProducer extends jspb.Message {
  getLittleEndianNumber(): IOProducer.LittleEndianNumber | undefined;
  setLittleEndianNumber(value?: IOProducer.LittleEndianNumber): void;
  hasLittleEndianNumber(): boolean;
  clearLittleEndianNumber(): void;
  hasLittleEndianNumber(): boolean;

  getDecimalNumber(): IOProducer.DecimalNumber | undefined;
  setDecimalNumber(value?: IOProducer.DecimalNumber): void;
  hasDecimalNumber(): boolean;
  clearDecimalNumber(): void;
  hasDecimalNumber(): boolean;

  getLiteral(): IOProducer.Literal | undefined;
  setLiteral(value?: IOProducer.Literal): void;
  hasLiteral(): boolean;
  clearLiteral(): void;
  hasLiteral(): boolean;

  getProducerCase(): IOProducer.ProducerCase;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): IOProducer.AsObject;
  static toObject(includeInstance: boolean, msg: IOProducer): IOProducer.AsObject;
  static serializeBinaryToWriter(message: IOProducer, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): IOProducer;
  static deserializeBinaryFromReader(message: IOProducer, reader: jspb.BinaryReader): IOProducer;
}

export namespace IOProducer {
  export type AsObject = {
    littleEndianNumber?: IOProducer.LittleEndianNumber.AsObject,
    decimalNumber?: IOProducer.DecimalNumber.AsObject,
    literal?: IOProducer.Literal.AsObject,
  }

  export class LittleEndianNumber extends jspb.Message {
    getNumber(): number;
    setNumber(value: number): void;

    getWidth(): number;
    setWidth(value: number): void;

    serializeBinary(): Uint8Array;
    toObject(includeInstance?: boolean): LittleEndianNumber.AsObject;
    static toObject(includeInstance: boolean, msg: LittleEndianNumber): LittleEndianNumber.AsObject;
    static serializeBinaryToWriter(message: LittleEndianNumber, writer: jspb.BinaryWriter): void;
    static deserializeBinary(bytes: Uint8Array): LittleEndianNumber;
    static deserializeBinaryFromReader(message: LittleEndianNumber, reader: jspb.BinaryReader): LittleEndianNumber;
  }

  export namespace LittleEndianNumber {
    export type AsObject = {
      number: number,
      width: number,
    }
  }


  export class DecimalNumber extends jspb.Message {
    getNumber(): number;
    setNumber(value: number): void;

    serializeBinary(): Uint8Array;
    toObject(includeInstance?: boolean): DecimalNumber.AsObject;
    static toObject(includeInstance: boolean, msg: DecimalNumber): DecimalNumber.AsObject;
    static serializeBinaryToWriter(message: DecimalNumber, writer: jspb.BinaryWriter): void;
    static deserializeBinary(bytes: Uint8Array): DecimalNumber;
    static deserializeBinaryFromReader(message: DecimalNumber, reader: jspb.BinaryReader): DecimalNumber;
  }

  export namespace DecimalNumber {
    export type AsObject = {
      number: number,
    }
  }


  export class Literal extends jspb.Message {
    getLiteral(): string;
    setLiteral(value: string): void;

    serializeBinary(): Uint8Array;
    toObject(includeInstance?: boolean): Literal.AsObject;
    static toObject(includeInstance: boolean, msg: Literal): Literal.AsObject;
    static serializeBinaryToWriter(message: Literal, writer: jspb.BinaryWriter): void;
    static deserializeBinary(bytes: Uint8Array): Literal;
    static deserializeBinaryFromReader(message: Literal, reader: jspb.BinaryReader): Literal;
  }

  export namespace Literal {
    export type AsObject = {
      literal: string,
    }
  }


  export enum ProducerCase { 
    PRODUCER_NOT_SET = 0,
    LITTLE_ENDIAN_NUMBER = 1,
    DECIMAL_NUMBER = 2,
    LITERAL = 3,
  }
}

export class IOConsumerSync extends jspb.Message {
  getChainId(): number;
  setChainId(value: number): void;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): IOConsumerSync.AsObject;
  static toObject(includeInstance: boolean, msg: IOConsumerSync): IOConsumerSync.AsObject;
  static serializeBinaryToWriter(message: IOConsumerSync, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): IOConsumerSync;
  static deserializeBinaryFromReader(message: IOConsumerSync, reader: jspb.BinaryReader): IOConsumerSync;
}

export namespace IOConsumerSync {
  export type AsObject = {
    chainId: number,
  }
}

export class IOReadResult extends jspb.Message {
  getData(): string;
  setData(value: string): void;

  getAny(): IOReadResult.Any | undefined;
  setAny(value?: IOReadResult.Any): void;
  hasAny(): boolean;
  clearAny(): void;
  hasAny(): boolean;

  getLine(): IOReadResult.Line | undefined;
  setLine(value?: IOReadResult.Line): void;
  hasLine(): boolean;
  clearLine(): void;
  hasLine(): boolean;

  getRegex(): IOReadResult.Regex | undefined;
  setRegex(value?: IOReadResult.Regex): void;
  hasRegex(): boolean;
  clearRegex(): void;
  hasRegex(): boolean;

  getLiteral(): IOReadResult.Literal | undefined;
  setLiteral(value?: IOReadResult.Literal): void;
  hasLiteral(): boolean;
  clearLiteral(): void;
  hasLiteral(): boolean;

  getNchars(): IOReadResult.NChars | undefined;
  setNchars(value?: IOReadResult.NChars): void;
  hasNchars(): boolean;
  clearNchars(): void;
  hasNchars(): boolean;

  getNumber(): IOReadResult.Number | undefined;
  setNumber(value?: IOReadResult.Number): void;
  hasNumber(): boolean;
  clearNumber(): void;
  hasNumber(): boolean;

  getChain(): IOReadResult.Chain | undefined;
  setChain(value?: IOReadResult.Chain): void;
  hasChain(): boolean;
  clearChain(): void;
  hasChain(): boolean;

  getOneof(): IOReadResult.OneOf | undefined;
  setOneof(value?: IOReadResult.OneOf): void;
  hasOneof(): boolean;
  clearOneof(): void;
  hasOneof(): boolean;

  getDetailCase(): IOReadResult.DetailCase;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): IOReadResult.AsObject;
  static toObject(includeInstance: boolean, msg: IOReadResult): IOReadResult.AsObject;
  static serializeBinaryToWriter(message: IOReadResult, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): IOReadResult;
  static deserializeBinaryFromReader(message: IOReadResult, reader: jspb.BinaryReader): IOReadResult;
}

export namespace IOReadResult {
  export type AsObject = {
    data: string,
    any?: IOReadResult.Any.AsObject,
    line?: IOReadResult.Line.AsObject,
    regex?: IOReadResult.Regex.AsObject,
    literal?: IOReadResult.Literal.AsObject,
    nchars?: IOReadResult.NChars.AsObject,
    number?: IOReadResult.Number.AsObject,
    chain?: IOReadResult.Chain.AsObject,
    oneof?: IOReadResult.OneOf.AsObject,
  }

  export class Any extends jspb.Message {
    serializeBinary(): Uint8Array;
    toObject(includeInstance?: boolean): Any.AsObject;
    static toObject(includeInstance: boolean, msg: Any): Any.AsObject;
    static serializeBinaryToWriter(message: Any, writer: jspb.BinaryWriter): void;
    static deserializeBinary(bytes: Uint8Array): Any;
    static deserializeBinaryFromReader(message: Any, reader: jspb.BinaryReader): Any;
  }

  export namespace Any {
    export type AsObject = {
    }
  }


  export class Line extends jspb.Message {
    getInner(): IOReadResult | undefined;
    setInner(value?: IOReadResult): void;
    hasInner(): boolean;
    clearInner(): void;

    serializeBinary(): Uint8Array;
    toObject(includeInstance?: boolean): Line.AsObject;
    static toObject(includeInstance: boolean, msg: Line): Line.AsObject;
    static serializeBinaryToWriter(message: Line, writer: jspb.BinaryWriter): void;
    static deserializeBinary(bytes: Uint8Array): Line;
    static deserializeBinaryFromReader(message: Line, reader: jspb.BinaryReader): Line;
  }

  export namespace Line {
    export type AsObject = {
      inner?: IOReadResult.AsObject,
    }
  }


  export class Regex extends jspb.Message {
    getGroupsList(): Array<string>;
    setGroupsList(value: Array<string>): void;
    clearGroupsList(): void;
    addGroups(value: string, index?: number): void;

    serializeBinary(): Uint8Array;
    toObject(includeInstance?: boolean): Regex.AsObject;
    static toObject(includeInstance: boolean, msg: Regex): Regex.AsObject;
    static serializeBinaryToWriter(message: Regex, writer: jspb.BinaryWriter): void;
    static deserializeBinary(bytes: Uint8Array): Regex;
    static deserializeBinaryFromReader(message: Regex, reader: jspb.BinaryReader): Regex;
  }

  export namespace Regex {
    export type AsObject = {
      groupsList: Array<string>,
    }
  }


  export class Literal extends jspb.Message {
    serializeBinary(): Uint8Array;
    toObject(includeInstance?: boolean): Literal.AsObject;
    static toObject(includeInstance: boolean, msg: Literal): Literal.AsObject;
    static serializeBinaryToWriter(message: Literal, writer: jspb.BinaryWriter): void;
    static deserializeBinary(bytes: Uint8Array): Literal;
    static deserializeBinaryFromReader(message: Literal, reader: jspb.BinaryReader): Literal;
  }

  export namespace Literal {
    export type AsObject = {
    }
  }


  export class NChars extends jspb.Message {
    serializeBinary(): Uint8Array;
    toObject(includeInstance?: boolean): NChars.AsObject;
    static toObject(includeInstance: boolean, msg: NChars): NChars.AsObject;
    static serializeBinaryToWriter(message: NChars, writer: jspb.BinaryWriter): void;
    static deserializeBinary(bytes: Uint8Array): NChars;
    static deserializeBinaryFromReader(message: NChars, reader: jspb.BinaryReader): NChars;
  }

  export namespace NChars {
    export type AsObject = {
    }
  }


  export class Number extends jspb.Message {
    serializeBinary(): Uint8Array;
    toObject(includeInstance?: boolean): Number.AsObject;
    static toObject(includeInstance: boolean, msg: Number): Number.AsObject;
    static serializeBinaryToWriter(message: Number, writer: jspb.BinaryWriter): void;
    static deserializeBinary(bytes: Uint8Array): Number;
    static deserializeBinaryFromReader(message: Number, reader: jspb.BinaryReader): Number;
  }

  export namespace Number {
    export type AsObject = {
    }
  }


  export class Chain extends jspb.Message {
    getInnerList(): Array<IOReadResult>;
    setInnerList(value: Array<IOReadResult>): void;
    clearInnerList(): void;
    addInner(value?: IOReadResult, index?: number): IOReadResult;

    serializeBinary(): Uint8Array;
    toObject(includeInstance?: boolean): Chain.AsObject;
    static toObject(includeInstance: boolean, msg: Chain): Chain.AsObject;
    static serializeBinaryToWriter(message: Chain, writer: jspb.BinaryWriter): void;
    static deserializeBinary(bytes: Uint8Array): Chain;
    static deserializeBinaryFromReader(message: Chain, reader: jspb.BinaryReader): Chain;
  }

  export namespace Chain {
    export type AsObject = {
      innerList: Array<IOReadResult.AsObject>,
    }
  }


  export class OneOf extends jspb.Message {
    getIndex(): number;
    setIndex(value: number): void;

    getInner(): IOReadResult | undefined;
    setInner(value?: IOReadResult): void;
    hasInner(): boolean;
    clearInner(): void;

    serializeBinary(): Uint8Array;
    toObject(includeInstance?: boolean): OneOf.AsObject;
    static toObject(includeInstance: boolean, msg: OneOf): OneOf.AsObject;
    static serializeBinaryToWriter(message: OneOf, writer: jspb.BinaryWriter): void;
    static deserializeBinary(bytes: Uint8Array): OneOf;
    static deserializeBinaryFromReader(message: OneOf, reader: jspb.BinaryReader): OneOf;
  }

  export namespace OneOf {
    export type AsObject = {
      index: number,
      inner?: IOReadResult.AsObject,
    }
  }


  export enum DetailCase { 
    DETAIL_NOT_SET = 0,
    ANY = 2,
    LINE = 3,
    REGEX = 4,
    LITERAL = 5,
    NCHARS = 6,
    NUMBER = 7,
    CHAIN = 8,
    ONEOF = 9,
  }
}

export class IOServerResponse extends jspb.Message {
  getAck(): boolean;
  setAck(value: boolean): void;
  hasAck(): boolean;

  getResult(): IOReadResult | undefined;
  setResult(value?: IOReadResult): void;
  hasResult(): boolean;
  clearResult(): void;
  hasResult(): boolean;

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
    result?: IOReadResult.AsObject,
  }

  export enum ResponseCase { 
    RESPONSE_NOT_SET = 0,
    ACK = 1,
    RESULT = 2,
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
  getTimestamp(): number;
  setTimestamp(value: number): void;

  getDriverOutput(): UIIODriverOutput | undefined;
  setDriverOutput(value?: UIIODriverOutput): void;
  hasDriverOutput(): boolean;
  clearDriverOutput(): void;
  hasDriverOutput(): boolean;

  getKindCase(): UIIODataUpdate.KindCase;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): UIIODataUpdate.AsObject;
  static toObject(includeInstance: boolean, msg: UIIODataUpdate): UIIODataUpdate.AsObject;
  static serializeBinaryToWriter(message: UIIODataUpdate, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): UIIODataUpdate;
  static deserializeBinaryFromReader(message: UIIODataUpdate, reader: jspb.BinaryReader): UIIODataUpdate;
}

export namespace UIIODataUpdate {
  export type AsObject = {
    timestamp: number,
    driverOutput?: UIIODriverOutput.AsObject,
  }

  export enum KindCase { 
    KIND_NOT_SET = 0,
    DRIVER_OUTPUT = 1,
  }
}

export class UIIODriverOutput extends jspb.Message {
  getData(): Uint8Array | string;
  getData_asU8(): Uint8Array;
  getData_asB64(): string;
  setData(value: Uint8Array | string): void;

  getIsStderr(): boolean;
  setIsStderr(value: boolean): void;

  serializeBinary(): Uint8Array;
  toObject(includeInstance?: boolean): UIIODriverOutput.AsObject;
  static toObject(includeInstance: boolean, msg: UIIODriverOutput): UIIODriverOutput.AsObject;
  static serializeBinaryToWriter(message: UIIODriverOutput, writer: jspb.BinaryWriter): void;
  static deserializeBinary(bytes: Uint8Array): UIIODriverOutput;
  static deserializeBinaryFromReader(message: UIIODriverOutput, reader: jspb.BinaryReader): UIIODriverOutput;
}

export namespace UIIODriverOutput {
  export type AsObject = {
    data: Uint8Array | string,
    isStderr: boolean,
  }
}

export enum IOConsumerInterpretation { 
  IOCI_INVALID = 0,
  IOCI_RAW = 1,
  IOCI_LITTLE_ENDIAN = 2,
  IOCI_DECIMAL = 3,
}

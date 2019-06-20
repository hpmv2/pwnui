import service_pb2 as pb


class IoOp(object):
    def __init__(self):
        self.interp = None

    def copy_from(self, other):
        self.interp = other.interp

    @property
    def raw(self):
        op = self.__class__()
        op.copy_from(self)
        op.interp = pb.IOCI_RAW
        return op

    def __rshift__(self, other):
        op = IoOp_Chain()
        op.chain.extend([self, other])
        return op

    def ToProto(self, proto):
        if self.interp:
            proto.interp = self.interp
        self.ToProtoInternal(proto)

    def ToProtoInternal(self, proto):
        raise NotImplementedError()

    def ExtractResults(self, result, builder):
        self.ExtractResultsInternal(result, builder)
        if self.interp:
            builder.Add(result.data, self.interp)

    def ExtractResultsInternal(self, result, builder):
        pass


class IoOp_Any(IoOp):
    def ToProtoInternal(self, proto):
        proto.any.SetInParent()


class IoOp_Line(IoOp):
    def __init__(self):
        super(IoOp_Line, self).__init__()
        self.matcher = None

    def copy_from(self, other):
        super(IoOp_Line, self).copy_from(other)
        self.matcher = other.matcher

    def __call__(self, matcher):
        op = IoOp_Line()
        op.copy_from(self)
        op.matcher = matcher
        return op

    def ToProtoInternal(self, proto):
        proto.line.SetInParent()
        if self.matcher:
            self.matcher.ToProto(proto.line.predicate)

    def ExtractResultsInternal(self, result, builder):
        if self.matcher:
            self.matcher.ExtractResults(result.line.inner, builder)


class IoOp_Number(IoOp):
    def ToProto(self, proto):
        proto.number.SetInParent()


class IoOp_Literal(IoOp):
    def __init__(self, lit=""):
        super(IoOp_Literal, self).__init__()
        self.lit = lit

    def copy_from(self, other):
        super(IoOp_Literal, self).copy_from(other)
        self.lit = other.lit

    def ToProtoInternal(self, proto):
        proto.literal.literal = self.lit


class IoOp_Chain(IoOp):
    def __init__(self):
        super(IoOp_Chain, self).__init__()
        self.chain = []

    def copy_from(self, other):
        super(IoOp_Chain, self).copy_from(other)
        self.chain = other.chain[:]

    def __rshift__(self, other):
        op = IoOp_Chain()
        op.copy_from(self)
        op.chain.append(other)
        return op

    def ToProtoInternal(self, proto):
        for child in self.chain:
            child.ToProto(proto.chain.consumers.add())

    def ExtractResultsInternal(self, result, builder):
        for i in range(len(self.chain)):
            self.chain[i].ExtractResults(result.chain.inner[i], builder)


any = IoOp_Any()
line = IoOp_Line()
number = IoOp_Number()
lit = IoOp_Literal


class IoChainBuilder(object):
    def __init__(self, chain_id, manager):
        self.chain_id = chain_id
        self.manager = manager
        self.sealed = False
        self.chain = IoOp_Chain()
        self.results = None

    def __rshift__(self, op):
        if self.sealed:
            raise Exception(
                'Cannot add more to a read chain once a subsequent chain is started or the chain is already synced.')
        self.chain = self.chain >> op
        self.manager.Read(self.chain_id, op)
        return self

    def __iter__(self):
        if self.results is None:
            self.results = self.manager.Sync(self.chain_id)
        builder = ResultsBuilder()
        self.chain.ExtractResults(self.results, builder)
        return iter(builder.Build())


class ResultsBuilder(object):
    def __init__(self):
        self.res = []

    def Add(self, data, interp):
        if interp == pb.IOCI_RAW:
            self.res.append(data)
        else:
            raise NotImplementedError('Unknown interpretation: ' + str(interp))

    def Build(self):
        return tuple(self.res)


class IoManager(object):
    def __init__(self, read_callback, write_callback, sync_callback):
        self.current_chain = None
        self.next_chain_id = 1
        self.read_callback = read_callback
        self.write_callback = write_callback
        self.sync_callback = sync_callback

    def __rshift__(self, op):
        if self.current_chain:
            self.current_chain.sealed = True
            self.current_chain = None
        chain_id = self.next_chain_id
        self.next_chain_id += 1
        self.current_chain = IoChainBuilder(chain_id, self)
        return self.current_chain >> op

    def Read(self, chain_id, op):
        assert chain_id == self.current_chain.chain_id
        proto = pb.IOConsumer()
        op.ToProto(proto)
        proto.chain_id = chain_id
        return self.read_callback(chain_id, proto)

    def Sync(self, chain_id):
        if self.current_chain.chain_id == chain_id:
            self.current_chain.sealed = True
            self.current_chain = None
        proto = pb.IOConsumerSync()
        proto.chain_id = chain_id
        return self.sync_callback(proto)

    def __lshift__(self, s):
        # TODO: support more than just raw string
        producer = pb.IOProducer()
        producer.literal.literal = s
        self.write_callback(producer)
        return self

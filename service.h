#ifndef SERVICE_H_
#define SERVICE_H_

#include "service.grpc.pb.h"

class UIServiceImpl : public UIService::Service {
public:
    grpc::Status NewSession(::grpc::ServerContext *context, const ::NewSessionRequest *request,
                            ::NewSessionResponse *response) override;
};

#endif // SERVICE_H_
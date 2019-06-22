import { Component, Input, OnInit, OnDestroy } from '@angular/core';
import { UIIODataRequest, UIIOStructuredOutput, UIIOStructuredInput, UIIORawOutput } from 'src/rpc/service_pb';
import { UIServiceClient } from 'src/rpc/ServiceServiceClientPb';

@Component({
  selector: 'app-iorenderer',
  templateUrl: './iorenderer.component.html',
  styleUrls: ['./iorenderer.component.scss'],
})
export class IORendererComponent implements OnInit, OnDestroy {
  public driverText: string = '';

  @Input() public sessionId: number;

  public consumersMap = new Map<number, UIIOStructuredOutput>();
  public consumers: number[] = [];
  public producers: UIIOStructuredInput[] = [];
  public rawOutputs: UIIORawOutput[] = [];

  constructor(private client: UIServiceClient) { }

  private binaryToAsciiText(binary: Uint8Array): string {
    let result = '';
    binary.forEach(c => {
      result += String.fromCharCode(c);
    });
    return result;
  }

  async ngOnInit() {
    let request = new UIIODataRequest();
    request.setSessionId(this.sessionId);
    console.log(request);
    this.client.getIODataForUI(request).on('data', (update) => {
      if (update.hasDriverOutput()) {
        this.driverText += this.binaryToAsciiText(
          update.getDriverOutput()!.getData_asU8());
      }
      if (update.hasStructuredOutput()) {
        let output = update.getStructuredOutput()!;
        if (this.consumersMap.has(output.getChainId())) {
          let consumer = this.consumersMap.get(output.getChainId())!;
          consumer.setReq(output.getReq());
          consumer.setProgress(output.getProgress());
        } else {
          this.consumersMap.set(output.getChainId(), output);
          this.consumers.push(output.getChainId());
        }
      }
      if (update.hasInput()) {
        this.producers.push(update.getInput()!);
      }
      if (update.getRawOutput()) {
        this.rawOutputs.push(update.getRawOutput()!);
      }
    });
  }

  ngOnDestroy(): void {
    // TODO: Cut the connection.
  }
}

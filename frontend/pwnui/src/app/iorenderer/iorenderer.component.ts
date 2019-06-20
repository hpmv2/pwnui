import { Component, Input, OnInit, OnDestroy } from '@angular/core';
import { UIIODataRequest } from 'src/rpc/service_pb';
import { UIServiceClient } from 'src/rpc/ServiceServiceClientPb';

@Component({
  selector: 'app-iorenderer',
  templateUrl: './iorenderer.component.html',
  styleUrls: ['./iorenderer.component.scss'],
})
export class IORendererComponent implements OnInit, OnDestroy {
  public driverText: string = '';

  @Input() public sessionId: number;

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
    });
  }

  ngOnDestroy(): void {
    // TODO: Cut the connection.
  }
}

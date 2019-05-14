import { Component, Input, OnInit, OnDestroy } from '@angular/core';
import { UIIOChunk, UIIODataRequest, UIIOOperation, UIIODataUpdate } from 'src/rpc/service_pb';
import { UIServiceClient } from 'src/rpc/ServiceServiceClientPb';

@Component({
  selector: 'app-iorenderer',
  templateUrl: './iorenderer.component.html',
  styleUrls: ['./iorenderer.component.scss'],
})
export class IORendererComponent implements OnInit, OnDestroy {
  public ioData: Array<UIIOOperation> = [];
  private chunks: Map<number, UIIOChunk> = new Map();

  @Input() public sessionId: number;

  constructor(private client: UIServiceClient) { }

  ngOnInit() {
    let request = new UIIODataRequest();
    request.setSessionId(this.sessionId);
    this.client.getIODataForUI(request).on('data', (update) => {
      let data = update.serializeBinary();
      update = UIIODataUpdate.deserializeBinary(data);
      if (update.hasAppendOperation()) {
        this.ioData.push(update.getAppendOperation()!);
        for (let chunk of update.getAppendOperation()!.getChunksList()) {
          this.chunks.set(chunk.getChunkId(), chunk);
        }
      } else if (update.hasUpdateChunk()) {
        let chunk = this.chunks.get(update.getUpdateChunk()!.getChunkId())!;
        chunk.clearElementsList();
        for (let element of update.getUpdateChunk()!.getElementsList()) {
          chunk.addElements(element);
        }
      }
    });
  }

  ngOnDestroy(): void {
    // TODO: Cut the connection.
  }
}

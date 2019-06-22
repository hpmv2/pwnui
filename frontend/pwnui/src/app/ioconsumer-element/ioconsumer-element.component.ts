import { Component, OnInit, Input } from '@angular/core';
import { IOReadResult, IOConsumer, IOConsumerInterpretation } from 'src/rpc/service_pb';

@Component({
  selector: 'ioconsumer-element',
  templateUrl: './ioconsumer-element.component.html',
  styleUrls: ['./ioconsumer-element.component.scss']
})
export class IOConsumerElementComponent {
  @Input()
  public req: IOConsumer;

  @Input()
  public progress: IOReadResult | null;

  get hasInterp() {
    return this.req.getInterp() != IOConsumerInterpretation.IOCI_INVALID;
  }

  chainInnerProgress(i: number): IOReadResult | null {
    if (this.progress == null) {
      return null;
    }
    let list = this.progress.getChain()!.getInnerList();
    if (i >= 0 && i < list.length) {
      return list[i];
    }
    return null;
  }
}

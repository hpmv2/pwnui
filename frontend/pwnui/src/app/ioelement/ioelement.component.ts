import { Component, Input } from '@angular/core';
import { UIIOStructuredOutput, UIIOStructuredInput, UIIORawOutput } from 'src/rpc/service_pb';

@Component({
  selector: 'ioelement',
  templateUrl: './ioelement.component.html',
  styleUrls: ['./ioelement.component.scss']
})
export class IOElementComponent {
  @Input()
  public consumer: UIIOStructuredOutput | null;

  @Input()
  public producer: UIIOStructuredInput | null;

  @Input()
  public rawOutput: UIIORawOutput | null;

  public binaryToAsciiText(binary: Uint8Array): string {
    let result = '';
    binary.forEach(c => {
      result += String.fromCharCode(c);
    });
    return result;
  }
}

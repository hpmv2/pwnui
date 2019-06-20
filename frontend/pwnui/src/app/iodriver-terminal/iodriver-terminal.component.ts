import { Component, Input } from '@angular/core';

@Component({
  selector: 'iodriver-terminal',
  templateUrl: './iodriver-terminal.component.html',
  styleUrls: ['./iodriver-terminal.component.scss']
})
export class IODriverTerminalComponent {
  // TODO: Very primitive for now...
  @Input() public text: string = '';
}

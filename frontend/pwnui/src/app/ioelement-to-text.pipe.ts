import { Pipe, PipeTransform } from '@angular/core';
import { UIIOElement, IOElementType } from 'src/rpc/service_pb';
import { IOElementText } from './ioelement-text';

@Pipe({
  name: 'ioElementToText'
})
export class IOElementToTextPipe implements PipeTransform {

  transform(value: UIIOElement): IOElementText {
    let data = value.getData_asU8();
    switch (value.getType()) {
      case IOElementType.IOET_RAW:
        return {
          text: this.binaryToAsciiText(data),
          cssClass: 'element-raw'
        };
      case IOElementType.IOET_UNINTERESTING:
        return {
          text: this.binaryToAsciiText(data),
          cssClass: 'element-uninteresting'
        };
      case IOElementType.IOET_UNPROCESSED:
        return {
          text: this.binaryToAsciiText(data),
          cssClass: 'element-unprocessed'
        };
      case IOElementType.IOET_HEX32:
        return {
          text: '0x' + new Uint32Array(value.getData_asU8().buffer.slice(0, 4))[0].toString(16),
          cssClass: 'element-hex32',
        };
      case IOElementType.IOET_DECIMAL32:
        return {
          text: parseInt(this.binaryToAsciiText(data)).toString(),
          cssClass: 'element-decimal32'
        };
    }
    return { text: '?', cssClass: '' };
  }

  private binaryToAsciiText(binary: Uint8Array): string {
    let result = '';
    binary.forEach(c => {
      result += String.fromCharCode(c);
    });
    return result;
  }
}

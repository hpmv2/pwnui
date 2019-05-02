import {Component} from '@angular/core';
import {NewSessionRequest} from '../rpc/service_pb';
import {UIServiceClient} from '../rpc/ServiceServiceClientPb';


@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss']
})
export class AppComponent {
  title = 'pwnui';

  newSession() {
    let client = new UIServiceClient('http://localhost:12346', {}, {});
    let request = new NewSessionRequest();
    client.newSession(request, {}, (err, response) => {
      console.log(response.getId());
    });
  }
}

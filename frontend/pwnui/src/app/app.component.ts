import {Component} from '@angular/core';
import {NewSessionRequest} from '../rpc/service_pb';
import {UIServiceClient} from '../rpc/ServiceServiceClientPb';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss'],
  providers: [{
    provide: UIServiceClient,
    useFactory: (comp: AppComponent) => comp.client,
    deps: [AppComponent]
  }]
})
export class AppComponent {
  public title = 'pwnui';
  public client = new UIServiceClient('http://localhost:12346', {}, {});

  newSession() {
    let request = new NewSessionRequest();
    this.client.newSession(request, {}, (err, response) => {
      console.log(response.getId());
    });
  }
}

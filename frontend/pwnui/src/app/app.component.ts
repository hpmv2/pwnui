import { Component } from '@angular/core';
import { NewSessionRequest } from '../rpc/service_pb';
import { UIServiceClient } from '../rpc/ServiceServiceClientPb';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss'],
  providers: [{
    provide: UIServiceClient,
    useFactory: getClientFromApp,
    deps: [AppComponent]
  }]
})
export class AppComponent {
  public title = 'pwnui';
  public client = new UIServiceClient('http://localhost:12346', {}, {});
  public code: string = '';
  editorOptions = { theme: 'vs-dark', language: 'python' };
  sessionId: number | null = null;

  newSession() {
    // TODO: terminate the previous session.
    let request = new NewSessionRequest();
    request.setBinary("/bin/cat");
    request.setScript(this.code);
    this.client.newSession(request, {}, (err, response) => {
      this.sessionId = response.getId();
    });
  }
}

export function getClientFromApp(comp: AppComponent) { return comp.client; }

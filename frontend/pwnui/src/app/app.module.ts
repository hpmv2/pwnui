import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';

import { AppComponent } from './app.component';
import { IORendererComponent } from './iorenderer/iorenderer.component';
import { MonacoEditorModule } from 'ngx-monaco-editor';
import { FormsModule } from '@angular/forms';
import { IODriverTerminalComponent } from './iodriver-terminal/iodriver-terminal.component';
import { IOElementComponent } from './ioelement/ioelement.component';
import { IOConsumerElementComponent } from './ioconsumer-element/ioconsumer-element.component';

@NgModule({
  declarations: [
    AppComponent,
    IORendererComponent,
    IODriverTerminalComponent,
    IOElementComponent,
    IOConsumerElementComponent,
  ],
  imports: [
    BrowserModule,
    FormsModule,
    MonacoEditorModule.forRoot(),
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }

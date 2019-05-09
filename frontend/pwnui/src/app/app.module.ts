import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';

import { AppComponent } from './app.component';
import { IORendererComponent } from './iorenderer/iorenderer.component';
import { IOElementToTextPipe } from './ioelement-to-text.pipe';

@NgModule({
  declarations: [
    AppComponent,
    IORendererComponent,
    IOElementToTextPipe
  ],
  imports: [
    BrowserModule
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }

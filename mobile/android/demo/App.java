package org.iota.mobile;

import android.app.*;

public class App extends Application {
	@Override
	public void onCreate() {
    super.onCreate();
    System.loadLibrary("hello_android");

	}
}

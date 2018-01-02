package org.iota.mobile;

import android.app.*;
import android.util.Log;

public class App extends Application {
	@Override
	public void onCreate() {
    super.onCreate();
    System.loadLibrary("dummy");

    Log.e("IOTA", "Library loaded.");
	}
}

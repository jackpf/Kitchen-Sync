package com.jackpf.kitchensync;

import com.jackpf.kitchensync.CInterface.CInterface;
import javafx.concurrent.Service;
import javafx.concurrent.Task;
import sun.awt.Mutex;

/**
 * Created by jackfarrelly on 26/01/2016.
 */
public class AnalyzerService extends Service<String> {
    private static final Mutex mutex = new Mutex();

    private CInterface cInterface = new CInterface();

    private final Info trackInfo;

    public AnalyzerService(Info trackInfo) {
        this.trackInfo = trackInfo;
    }

    @Override
    protected Task<String> createTask() {
        return new Task<String>() {
            @Override
            protected String call() throws Exception {
                mutex.lock();
                String bpm = cInterface.getBpm(trackInfo.getFilename());
                mutex.unlock();
                return bpm;
            }
        };
    }
}

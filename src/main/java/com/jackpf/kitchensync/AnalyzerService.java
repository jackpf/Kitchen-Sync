package com.jackpf.kitchensync;

import TrackAnalyzer.TrackAnalyzer;
import javafx.concurrent.Service;
import javafx.concurrent.Task;
import sun.awt.Mutex;

/**
 * Created by jackfarrelly on 26/01/2016.
 */
public class AnalyzerService extends Service<TrackAnalyzer.Info> {
    private static final Mutex mutex = new Mutex();

    private final Info trackInfo;

    public AnalyzerService(Info trackInfo) {
        this.trackInfo = trackInfo;
    }

    @Override
    protected Task<TrackAnalyzer.Info> createTask() {
        return new Task<TrackAnalyzer.Info>() {
            @Override
            protected TrackAnalyzer.Info call() throws Exception {
                mutex.lock();
                TrackAnalyzer analyzer = new TrackAnalyzer(new String[]{trackInfo.getFilename(), "-w", "-o", "/tmp/trackanalyzer.txt"});
                TrackAnalyzer.Info info = analyzer.analyzeTrack(trackInfo.getFilename(), false);
                mutex.unlock();
                return info;
            }
        };
    }
}

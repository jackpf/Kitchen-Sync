package com.jackpf.kitchensync.service;

import com.jackpf.kitchensync.CInterface.CInterface;
import com.jackpf.kitchensync.Executor;
import com.jackpf.kitchensync.entity.Info;
import com.jackpf.kitchensync.executor.FFMPEGExecutor;
import javafx.concurrent.Service;
import javafx.concurrent.Task;
import sun.awt.Mutex;

/**
 * Created by jackfarrelly on 26/01/2016.
 */
public class AnalyserService extends Service<Float> {
    private static final Mutex mutex = new Mutex();

    private CInterface cInterface;

    private Executor ffmpeg;

    private final Info trackInfo;

    public AnalyserService(Info trackInfo) throws Exception {
        this.cInterface = new CInterface();
        ffmpeg = new FFMPEGExecutor();
        this.trackInfo = trackInfo;
    }

    @Override
    protected Task<Float> createTask() {
        return new Task<Float>() {
            @Override
            protected Float call() throws Exception {
                mutex.lock();

                try {
                    String filename;

                    if (cInterface.hasDecoderFor(trackInfo.getFile().getAbsolutePath())) {
                        filename = trackInfo.getFile().getAbsolutePath();
                    } else {
                        ffmpeg.run(new String[]{"-i", trackInfo.getFile().getAbsolutePath(), trackInfo.getTmpFile().getAbsolutePath()});
                        filename = trackInfo.getTmpFile().getAbsolutePath();
                    }

                    //float bpm = cInterface.getBpm(trackInfo.getTmpFile().getAbsolutePath());
                    float bpm = cInterface.getBpm(filename);
                    float quality = cInterface.getQuality(filename);

                    trackInfo.setQuality(quality);

                    return bpm;
                } finally {
                    mutex.unlock();
                }
            }
        };
    }
}

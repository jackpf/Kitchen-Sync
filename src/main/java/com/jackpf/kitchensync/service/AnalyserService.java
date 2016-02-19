package com.jackpf.kitchensync.service;

import com.jackpf.kitchensync.CInterface.CInterface;
import com.jackpf.kitchensync.Executor;
import com.jackpf.kitchensync.entity.Info;
import com.jackpf.kitchensync.executor.FFMPEGExecutor;
import javafx.concurrent.Service;
import javafx.concurrent.Task;
import sun.awt.Mutex;

import java.io.IOException;

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
                    ffmpeg.run(new String[]{"-y", "-i", trackInfo.getFile().getAbsolutePath(), trackInfo.getTmpFile().getAbsolutePath()});

                    if (!trackInfo.getTmpFile().exists()) {
                        throw new IOException("File " + trackInfo.getTmpFile().getAbsolutePath() + " does not exist");
                    }

                    float bpm = cInterface.getBpm(trackInfo.getTmpFile().getAbsolutePath());

                    trackInfo.getTmpFile().delete();

                    ffmpeg.run(new String[]{"-i", trackInfo.getFile().getAbsolutePath(), "-ac", "1", trackInfo.getTmpFile2().getAbsolutePath()});

                    float quality = cInterface.getQuality(trackInfo.getTmpFile2().getAbsolutePath());
                    trackInfo.setQuality(quality);
                    System.out.println("Quality of " + trackInfo.getFilename() + ": " + quality);

                    trackInfo.getTmpFile2().delete();

                    return bpm;
                } finally {
                    mutex.unlock();
                }
            }
        };
    }
}

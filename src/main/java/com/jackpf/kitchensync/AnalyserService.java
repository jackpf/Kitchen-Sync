package com.jackpf.kitchensync;

import com.jackpf.kitchensync.CInterface.CInterface;
import javafx.concurrent.Service;
import javafx.concurrent.Task;
import sun.awt.Mutex;

import java.io.File;
import java.io.IOException;

/**
 * Created by jackfarrelly on 26/01/2016.
 */
public class AnalyserService extends Service<Float> {
    private final File TMP_FILE = new File("/tmp/tmp.wav");

    private static final Mutex mutex = new Mutex();

    private CInterface cInterface = new CInterface();

    private Executor ffmpeg;

    private final Info trackInfo;

    public AnalyserService(Info trackInfo) throws Exception {
        ffmpeg = new Executor("ffmpeg");
        this.trackInfo = trackInfo;
    }

    @Override
    protected Task<Float> createTask() {
        return new Task<Float>() {
            @Override
            protected Float call() throws Exception {
                mutex.lock();

                ffmpeg.run(new String[]{"-y", "-i", trackInfo.getFilename(), TMP_FILE.getAbsolutePath()});

                if (!TMP_FILE.exists()) {
                    throw new IOException("File " + TMP_FILE.getAbsolutePath() + " does not exist");
                }

                float bpm = cInterface.getBpm(TMP_FILE.getAbsolutePath());

                System.out.println("Raw bpm: " + bpm);

                TMP_FILE.delete();

                mutex.unlock();

                return bpm;
            }
        };
    }
}

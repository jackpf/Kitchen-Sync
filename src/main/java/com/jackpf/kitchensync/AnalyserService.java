package com.jackpf.kitchensync;

import com.jackpf.kitchensync.CInterface.CInterface;
import javafx.concurrent.Service;
import javafx.concurrent.Task;
import sun.awt.Mutex;

import java.io.IOException;
import java.net.URLDecoder;

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
        ffmpeg = new Executor(
            URLDecoder.decode(ClassLoader.getSystemClassLoader().getResource("ffmpeg").getPath(), "UTF-8")
        );
        this.trackInfo = trackInfo;
    }

    @Override
    protected Task<Float> createTask() {
        return new Task<Float>() {
            @Override
            protected Float call() throws Exception {
                // Needs to be smart mutex!
                mutex.lock();

                ffmpeg.run(new String[]{"-y", "-i", trackInfo.getFile().getAbsolutePath(), trackInfo.getTmpFile().getAbsolutePath()});

                if (!trackInfo.getTmpFile().exists()) {
                    throw new IOException("File " + trackInfo.getTmpFile().getAbsolutePath() + " does not exist");
                }

                float bpm = cInterface.getBpm(trackInfo.getTmpFile().getAbsolutePath());

                trackInfo.getTmpFile().delete();

                ffmpeg.run(new String[]{"-y", "-i", trackInfo.getFile().getAbsolutePath(), "-ac", "1", trackInfo.getTmpFile2().getAbsolutePath()});

                float quality = cInterface.getQuality(trackInfo.getTmpFile2().getAbsolutePath());
                System.out.println("Quality of " + trackInfo.getFilename() + ": " + quality);

                trackInfo.getTmpFile2().delete();

                mutex.unlock();

                return bpm;
            }
        };
    }
}

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
public class ProcessorService extends Service<Info> {
    private static final Mutex mutex = new Mutex();

    private final File TMP_FILE = new File("/tmp/tmp.wav"), TMP_FILE2 = new File("/tmp/tmp2.wav");

    private CInterface cInterface = new CInterface();

    private Executor ffmpeg;

    private final Info trackInfo;

    private final float targetBpm;

    private final File outputDir;

    public ProcessorService(Info trackInfo, float targetBpm, File outputDir) throws Exception {
        this.trackInfo = trackInfo;
        this.targetBpm = targetBpm;
        this.outputDir = outputDir;

        ffmpeg = new Executor("/usr/local/bin/ffmpeg");
    }

    @Override
    protected Task<Info> createTask() {
        return new Task<Info>() {
            @Override
            protected Info call() throws Exception {
                mutex.lock();

                ffmpeg.run(new String[]{"-y", "-i", trackInfo.getFile().getAbsolutePath(), TMP_FILE.getAbsolutePath()});

                if (!TMP_FILE.exists()) {
                    throw new IOException("File " + TMP_FILE.getAbsolutePath() + " does not exist");
                }

                cInterface.setBpm(
                    TMP_FILE.getAbsolutePath(),
                    TMP_FILE2.getAbsolutePath(),
                    Float.parseFloat(trackInfo.getBpm()),
                    targetBpm
                );

                TMP_FILE.delete();

                if (!TMP_FILE2.exists()) {
                    throw new IOException("File " + TMP_FILE2.getAbsolutePath() + " does not exist");
                }

                ffmpeg.run(new String[]{"-y", "-i", TMP_FILE2.getAbsolutePath(), "-ab", "320k", "-ac", "2", outputDir.getParent() + "/" + trackInfo.getFile().getName()});

                TMP_FILE2.delete();

                mutex.unlock();

                return trackInfo;
            }
        };
    }
}

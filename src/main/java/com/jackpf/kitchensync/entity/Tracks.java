package com.jackpf.kitchensync.entity;

import javafx.collections.ObservableList;
import javafx.stage.FileChooser;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;

/**
 * Created by jackfarrelly on 19/02/2016.
 */
public class Tracks {
    private ObservableList<Info> tracks;

    public Tracks(ObservableList<Info> tracks) {
        this.tracks = tracks;
    }

    public Info addFile(File file) throws IOException {
        if (!file.getName().toLowerCase().endsWith(".mp3") && !file.getName().toLowerCase().endsWith(".wav")) {
            return null;
        }

        Info trackInfo = new Info(file, Info.NO_BPM);

        for (Info track : tracks) {
            if (track.getFile().getAbsolutePath().equals(trackInfo.getFile().getAbsolutePath())) {
                return null;
            }
        }

        if (!file.getAbsolutePath().matches("\\A\\p{ASCII}*\\z")) {
            throw new IOException(file.getName() + " contains non ascii characters. This version of Kitchen Sync can not process this file, rename the file to add it.");
        }

        tracks.add(trackInfo);

        return trackInfo;
    }

    public void updateOutliers() {
        final String ATN_STR = " - ?";

        float totalBpm = 0;
        int count = 0;

        for (Info track : tracks) {
            try {
                totalBpm += Float.parseFloat(track.getBpm());
                count++;
            } catch (NumberFormatException e) {
                continue;
            }
        }

        float avg = totalBpm / count;

        for (Info track : tracks) {
            try {
                float bpm = Float.parseFloat(track.getBpm().replace(ATN_STR, ""));

                if ((bpm > avg + avg * 0.1 || bpm < avg - avg * 0.1) && !track.getBpm().contains(ATN_STR)) {
                    track.setBpm(track.getBpm() + ATN_STR);
                } else if (track.getBpm().contains(ATN_STR) && (bpm <= avg + avg * 0.1 && bpm >= avg - avg * 0.1)) {
                    track.setBpm(track.getBpm().replace(ATN_STR, ""));
                }
            } catch (NumberFormatException e) {
                continue;
            }
        }
    }

    public FileChooser.ExtensionFilter getExtensionFilter() {
        return new FileChooser.ExtensionFilter("Audio files", new ArrayList<>(Arrays.asList("*.mp3", "*.wav")));
    }

    public ObservableList<Info> getItems() {
        return tracks;
    }
}

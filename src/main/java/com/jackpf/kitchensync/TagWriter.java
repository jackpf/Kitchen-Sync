package com.jackpf.kitchensync;

import com.jackpf.kitchensync.entity.Info;

import java.util.Arrays;

/**
 * Created by jackfarrelly on 07/02/2016.
 */
public class TagWriter {
    public static class Tags {
        public final String name;
        public final String artist;
        public Tags(String name, String artist) {
            this.name = name;
            this.artist = artist;
        }
    }

    private Info track;

    public TagWriter(Info track) {
        this.track = track;
    }

    public Tags getTags() {
        String filename = track.getFile().getName();
        String[] parts = filename.split("-");

        String artist = parts[0], name = "";

        if (parts.length > 1) {
            name = String.join("-", Arrays.copyOfRange(parts, 1, parts.length))
                .replaceAll("((http|https):\\/\\/)?[-a-zA-Z0-9@:%._\\+~#=]{2,256}\\.[a-z]{2,6}\\b([-a-zA-Z0-9@:%_\\+.~#?&//=]*)", "");
        }

        return new Tags(name, artist);
    }
}

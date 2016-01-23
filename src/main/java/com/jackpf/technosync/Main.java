package com.jackpf.technosync;

import TrackAnalyzer.TrackAnalyzer;

/**
 * Created by jackfarrelly on 23/01/2016.
 */
public class Main {
    public static void main(String[] args) {
        String f = "/Users/jackfarrelly/Downloads/secluded.mp3";

        try {
            new TrackAnalyzer(new String[]{f, "-w", "-o", "/Users/jackfarrelly/results.txt"})
                .analyzeTrack(f, false);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}

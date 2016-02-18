package com.jackpf.kitchensync.CInterface;

/**
 * Created by jackfarrelly on 27/01/2016.
 */
public class CInterface {
    public native String getVersion();
    public native float getBpm(String filename);
    public native void setBpm(String inFilename, String outFilename, float fromBpm, float toBpm);

    public CInterface() {//static {
        try {
            System.loadLibrary("kitchensync");
        } catch (UnsatisfiedLinkError e) {
            throw new RuntimeException("Unable to load kitchensync library", e);
        }
    }

    public static void main(String[] args) {
        if (args.length < 3) {
            throw new RuntimeException("Usage: CInterface <inFile> <outFile> <targetBpm>");
        }

        CInterface cInterface = new CInterface();

        String version = cInterface.getVersion();
        System.out.println("Soundtouch lib version: " + version);

        float bpm = cInterface.getBpm(args[0]);

        System.out.println("BPM of " + args[0] + ": "  + bpm);

        cInterface.setBpm(args[0], args[1], bpm, Float.parseFloat(args[2]));

        System.out.println("Done");
    }
}

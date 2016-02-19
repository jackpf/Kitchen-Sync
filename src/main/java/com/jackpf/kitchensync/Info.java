package com.jackpf.kitchensync;

import javafx.beans.property.SimpleStringProperty;
import javafx.concurrent.Service;
import org.apache.commons.codec.digest.DigestUtils;

import java.io.File;
import java.io.IOException;
import java.sql.Timestamp;
import java.util.Date;

/**
 * Created by jackfarrelly on 25/01/2016.
 */
public class Info {
    public static final String NO_BPM = "-";

    private final SimpleStringProperty filename = new SimpleStringProperty();
    private final SimpleStringProperty displayName = new SimpleStringProperty();
    private final SimpleStringProperty bpm = new SimpleStringProperty();

    private final File file;

    private final File tmpFile, tmpFile2;

    private Service service;

    private float quality = -1.0f;

    public Info(File file, String bpm) throws IOException {
        this.file = file;
        String hash = DigestUtils.sha1Hex(file.getPath() + new Timestamp(new Date().getTime()) + Math.random());
        // Using java's generated tmp files causes issues when reading from kitchensync librarys
        //this.tmpFile = File.createTempFile(hash, ".wav");
        this.tmpFile = new File("/tmp/" + hash + ".wav");
        //this.tmpFile2 = File.createTempFile(hash + "-2", ".wav");
        this.tmpFile2 = new File("/tmp/" + hash + "-2.wav");

        setFilename(file.getAbsolutePath());
        setDisplayName(file.getName());
        setBpm(bpm);
    }

    public String getFilename() {
        return filename.get();
    }

    public SimpleStringProperty filenameProperty() {
        return filename;
    }

    public void setFilename(String filename) {
        this.filename.set(filename);
    }

    public String getDisplayName() {
        return displayName.get();
    }

    public SimpleStringProperty displayNameProperty() {
        return displayName;
    }

    public void setDisplayName(String displayName) {
        this.displayName.set(displayName);
    }

    public String getBpm() {
        return bpm.get();
    }

    public SimpleStringProperty bpmProperty() {
        return bpm;
    }

    public void setBpm(String bpm) {
        this.bpm.set(bpm);
    }

    public File getFile() {
        return file;
    }

    public File getTmpFile() {
        return tmpFile;
    }

    public File getTmpFile2() {
        return tmpFile2;
    }

    public Service getService() {
        return service;
    }

    public void setService(Service service) {
        this.service = service;
    }

    public float getQuality() {
        return quality;
    }

    public void setQuality(float quality) {
        this.quality = quality;
    }
}

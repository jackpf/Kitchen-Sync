package com.jackpf.kitchensync;

import javafx.beans.property.SimpleStringProperty;
import javafx.concurrent.Service;

import java.io.File;

/**
 * Created by jackfarrelly on 25/01/2016.
 */
public class Info {
    public static final String NO_BPM = "-";

    private final SimpleStringProperty filename = new SimpleStringProperty();
    private final SimpleStringProperty displayName = new SimpleStringProperty();
    private final SimpleStringProperty bpm = new SimpleStringProperty();

    private final File file;

    private Service service;

    public Info(File file, String bpm) {
        this.file = file;
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

    public Service getService() {
        return service;
    }

    public void setService(Service service) {
        this.service = service;
    }
}

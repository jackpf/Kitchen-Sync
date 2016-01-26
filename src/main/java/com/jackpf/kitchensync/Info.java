package com.jackpf.kitchensync;

import javafx.beans.property.SimpleStringProperty;

/**
 * Created by jackfarrelly on 25/01/2016.
 */
public class Info {
    public final SimpleStringProperty filename = new SimpleStringProperty();
    public final SimpleStringProperty bpm = new SimpleStringProperty();

    public Info(String filename, String bpm) {
        setFilename(filename);
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

    public String getBpm() {
        return bpm.get();
    }

    public SimpleStringProperty bpmProperty() {
        return bpm;
    }

    public void setBpm(String bpm) {
        this.bpm.set(bpm);
    }
}

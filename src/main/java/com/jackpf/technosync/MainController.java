package com.jackpf.technosync;

import TrackAnalyzer.TrackAnalyzer;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.TableView;

/**
 * Created by jackfarrelly on 25/01/2016.
 */
public class MainController {
    @FXML
    private TableView<TrackAnalyzer.Info> tableView;

    @FXML
    protected void addTrack(ActionEvent event) {
        ObservableList<TrackAnalyzer.Info> data = tableView.getItems();
        TrackAnalyzer.Info info = new TrackAnalyzer.Info("test", 130, "3E");
        data.add(info);
    }
}

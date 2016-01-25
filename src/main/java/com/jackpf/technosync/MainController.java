package com.jackpf.technosync;

import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.cell.TextFieldTableCell;

/**
 * Created by jackfarrelly on 25/01/2016.
 */
public class MainController {
    @FXML
    private TableView<Info> tracks;

    @FXML
    private TableColumn bpm;

    public MainController() {
    }

    @FXML
    protected void addTrack(ActionEvent event) {
        ObservableList<Info> data = tracks.getItems();
        Info info = new Info("test", "130");
        data.add(info);
        bpm.setCellFactory(TextFieldTableCell.<Info>forTableColumn());
    }
}

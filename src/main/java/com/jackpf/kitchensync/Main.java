package com.jackpf.kitchensync;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.stage.Stage;

import java.io.IOException;

/**
 * Created by jackfarrelly on 23/01/2016.
 */
public class Main extends Application {
    public static void main(String[] args) {
        launch(args);
    }

    @Override
    public void start(Stage stage) throws IOException {
        FXMLLoader loader = new FXMLLoader(getClass().getResource("/layout/main.fxml"));
        Parent root = loader.load();
        Controller controller = loader.getController();
        controller.initialise(root, stage);
    }
}

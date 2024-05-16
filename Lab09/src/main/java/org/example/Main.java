package org.example;

import java.io.IOException;
import java.util.List;
import java.util.concurrent.Executors;

public class Main {
    private static List<String> productTypes = List.of("Electronics", "Clothing", "Books");

    public static void main(String[] args) {
        var warehouse = new Warehouse(Main.productTypes);

        final int producersCount = 2;
        final int consumersCount = 2;

        var producersExecutor = Executors.newFixedThreadPool(producersCount);
        var consumersExecutor = Executors.newFixedThreadPool(consumersCount);

        for (int i = 0; i < producersCount; i++) {
            producersExecutor.execute(new Producer(warehouse));
        }

        for (int i = 0; i < consumersCount; i++) {
            consumersExecutor.execute(new Consumer(warehouse));
        }

        // Wait for enter to shutdown the threads and stop application
        try {
            System.in.read();
        } catch (IOException e) {
            e.printStackTrace();
        }

        producersExecutor.shutdown();
        consumersExecutor.shutdown();
    }
}
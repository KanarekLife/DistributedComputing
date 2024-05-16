package org.example;

import java.util.List;
import java.util.concurrent.ThreadLocalRandom;

public class Producer implements Runnable {
    private List<String> productTypes;
    private Warehouse warehouse;

    public Producer(Warehouse warehouse) {
        this.productTypes = warehouse.getProductTypes();
        this.warehouse = warehouse;
    }

    @Override
    public void run() {
        final ThreadLocalRandom random = ThreadLocalRandom.current();
        while (true) {
            // Produce a random product type
            String productType = productTypes.get(random.nextInt(productTypes.size()));

            // Produce a random quantity
            int quantity = random.nextInt(10) + 1;

            // Try to add the product to the warehouse
            boolean added = warehouse.tryToAddProduct(productType, quantity);

            if (added) {
                System.out.println("Produced: " + quantity + " " + productType);
            } else {
                System.out.println("Failed to produce: " + quantity + " " + productType);
            }

            // Sleep for a random interval
            try {
                Thread.sleep(random.nextInt(2000) + 1000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}

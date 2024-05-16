package org.example;

import java.util.List;
import java.util.concurrent.ThreadLocalRandom;

public class Consumer implements Runnable {
    private List<String> productTypes;
    private Warehouse warehouse;

    public Consumer(Warehouse warehouse) {
        this.productTypes = warehouse.getProductTypes();
        this.warehouse = warehouse;
    }

    @Override
    public void run() {
        final ThreadLocalRandom random = ThreadLocalRandom.current();
        while (true) {
            // Consume a random product type
            String productType = productTypes.get(random.nextInt(productTypes.size()));

            // Consume a random quantity
            int quantity = random.nextInt(5) + 1;

            // Try to remove the product from the warehouse
            boolean removed = warehouse.tryToRemoveProduct(productType, quantity);

            if (removed) {
                System.out.println("Consumed: " + quantity + " " + productType);
            } else {
                System.out.println("Failed to consume: " + quantity + " " + productType);
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

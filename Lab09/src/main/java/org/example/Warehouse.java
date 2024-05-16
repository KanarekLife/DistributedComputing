package org.example;

import java.util.HashMap;
import java.util.List;

public class Warehouse {
    private final List<String> productTypes;
    private HashMap<String, Integer> products = new HashMap<>();
    private int capacity;

    public Warehouse(List<String> productTypes) {
        this.productTypes = productTypes;
        this.capacity = 100;
    }

    public Warehouse(List<String> productTypes, int capacity) {
        this.productTypes = productTypes;
        this.capacity = capacity;
    }

    public synchronized boolean tryToAddProduct(String productType, int quantity) {
        if (!productTypes.contains(productType)) {
            return false;
        }

        if (products.getOrDefault(productType, 0) + quantity > capacity) {
            return false;
        }

        products.put(productType, products.getOrDefault(productType, 0) + quantity);
        System.out.println("Produced: " + quantity + " " + productType);
        return true;
    }

    public synchronized boolean tryToRemoveProduct(String productType, int quantity) {
        if (!productTypes.contains(productType)) {
            return false;
        }

        if (products.getOrDefault(productType, 0) == 0) {
            System.out.println("Failed to consume: " + quantity + " " + productType);
            return false;
        }

        if (products.getOrDefault(productType, 0) < quantity) {
            var present = products.getOrDefault(productType, 0);
            products.put(productType, 0);
            System.out.println("Consumed: " + present + " out of " + quantity + " " + productType);
            return true;
        }

        products.put(productType, products.getOrDefault(productType, 0) - quantity);
        System.out.println("Consumed: " + quantity + " " + productType);
        return true;
    }

    public List<String> getProductTypes() {
        return productTypes;
    }
}

package com.company;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;
import java.util.UUID;

public class Order
{
    private String orderId;
    private List<Box> packages;

    public Order(Box box)
    {
        packages = new ArrayList<>();
        orderId = UUID.randomUUID().toString();
    }

    public void add(Box box)
    {
        packages.add(box);
    }

    public String getOrderId()
    {
        return orderId;
    }
}

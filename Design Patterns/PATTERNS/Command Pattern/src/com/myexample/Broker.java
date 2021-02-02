package com.myexample;

import java.util.List;

public class Broker
{
    private List<Order> orders;

    public Broker(List<Order> ordersToProcess)
    {
        orders = ordersToProcess;
    }
    public void takeOrder(Order order)
    {
        orders.add(order);
    }
    public void placeOrders()
    {
        for(Order order : orders)
        {
            order.execute();
        }
    }

}

package com.company;

public class Receipt
{
    private float grandTotal;
    private String id;

    public Receipt(Order order)
    {
        id = order.getOrderId();
    }

    public int getGrandTotal(Box mainBox)
    {
        int total = 0;
        for (int i = 0; i < mainBox.boxes.size(); i++) {
            for (int j = 0; j < mainBox.boxes.get(i).getProducts().size(); j++) {
                total += mainBox.boxes.get(i).getProducts().get(j).price;
            }
        }
        return total;
    }

    public String getId()
    {
        return id;
    }

}

package com.company;

public class Main
{


    public static void main(String[] args)
    {
        Box mainBox = new Box("Main");
        Order order = new Order(mainBox);
        Receipt receipt = new Receipt(order);
        Product phone = new Product("phone", 1500);
        Product tablet = new Product("tablet", 800);
        Product phoneCharger = new Product("phone charger", 25);
        Product tabletCase = new Product("tablet case", 50);

        order.add(mainBox);

        Box phoneBox = new Box("phoneBox");
        phoneBox.add(phoneCharger);
        Box tabletBox = new Box("tabletBox");
        tabletBox.add(tabletCase);


        mainBox.add(phoneBox);
        mainBox.add(tabletBox);
        phoneBox.add(phone);

        System.out.println(receipt.getGrandTotal(mainBox));

    }
}

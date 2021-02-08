package com.company;

public class Product
{
    private String name;
    protected int price;

    public Product(String name, int price)
    {
        this.name = name;
        this.price = price;
    }

    public String getInfo()
    {
        return ("Product: [Name : " + name + ", Price:  " + price + "]");
    }


}

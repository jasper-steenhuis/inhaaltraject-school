package com.company;

import java.util.ArrayList;
import java.util.List;

public class Box
{
    private List<Product> productsList;
    protected List<Box> boxes;
    private String name;

    public Box(String name)
    {
        this.name = name;
        productsList = new ArrayList<>();
        boxes = new ArrayList<>();
    }

    public void add(Product p)
    {
        productsList.add(p);
    }

    public void add(Box b)
    {
        boxes.add(b);
    }

    public void remove(Product p)
    {
        productsList.remove(p);
    }

    public List<Product> getProducts()
    {
        return productsList;
    }

}

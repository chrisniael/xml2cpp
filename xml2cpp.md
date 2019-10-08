

```xml
id="127.0.0.1"
```

```cpp
pugi::xml_attribute id;
bool Init(const pugi::xml_node& xml_node) {
    this->id = xml_node.attribute("id");
    return true;
}
```




```xml
<name>gate</name>
```

```cpp
pugi::xml_text name_;
pugi::xml_text name() const { return this->name_; }

bool Init(const pugi::xml_node& xml_node) {
    
    
    pugi::xml_node name_node = xml_node.child("name");
    if (!name_node) {
    	reutrn false;
    }
    pugi::xml_node plain_data_node = name_node.first_child();
    if (!plain_data_node) {
        return false;
    }
    this->name_ = plain_data_node.text();
    
    
}
```






```xml
<item ip="127.0.0.1" />
```

```cpp
struct _item_ {
    
    pugi::xml_attribute id


    bool Init(const pugi::xml_node& xml_node) {
        
        
        
        this->id = xml_node.attribute("id");
        return true;
        

    }
    
    
    
};
_item_ item_;
const _item_& item() const { return this->item_; }

bool Init(const pugi::xml_node& xml_node) {
    
    pugi::xml_node item_node = xml_node.child("item");
    if (!item_node) {
        return false;
    }
    if (!this->item_.Init(item_node)) {
        return false;
    }
    
    return true;
}
```







```xml
<item ip="127.0.0.1">
  <name>test</name>
</item>
```

```cpp
struct _item_ {
    pugi::xml_attribute id;
    pugi::xml_text name_;
    pugi::xml_text name() const { return this->name_; }

    bool Init(const pugi::xml_node& xml_node) {
        this->id = xml_node.attribute("id");

        pugi::xml_node name_node = xml_node.child("name");
        if (!name_node) {
            reutrn false;
        }
        pugi::xml_node plain_data_node = name_node.first_child();
        if (!plain_data_node) {
            return false;
        }
        this->name_ = plain_data_node.text();

        return true;
    }
};
_item_ item_;
const _item_& item() const { return this->item_; }
```










```xml
<item ip="127.0.0.1" port="1235"/>
<item ip="127.0.0.1" port="1235"/>
```

```cpp
struct _item_ {
    pugi::xml_attribute ip;

    bool Init(const pugi::xml_node& xml_node) {
        this->id = xml_node.attribute("ip");
        return true;
    }
};
std::vector<_item_> item_;
const std::vector<_item_>& item() const { return this->item_; }


bool Init(const pugi::xml_node& xml_node) {

    pugi::xml_node item_node = xml_node.child("item");
    while (item_node) {
        _item_ item;
        item.Init(item_node);
        this->item_.push_back(item);
        item_node = item_node.next_sibling("item");
    }
    return true;

}
```





```xml
<name>abc</name>
<name>zxc</name>
```

```cpp
std::vector<pugi::xml_text> name_;
std::vector<pugi::xml_text> name() const { return this->name_; }

bool Init(const pugi::xml_node& xml_node) {
    
    
    pugi::xml_node name_node = xml_node.child("name");
    while (name_node) {
        pugi::xml_node plain_data_node = name_node.first_child();
        if (!plain_data_node) {
            return false;
        }
        pugi::xml_text plain_data_text = plain_data_node.text();
        this->name_.push_back(plain_data_text);
        name_node = name_node.next_sibling("name");
    }
    
    
}
```








```xml
<item ip="127.0.0.1" port="1235">
    <ip>127.0.0.1</ip>
    <port>92</port>
</item>
<item ip="127.0.0.1" port="1235">
    <ip>127.0.0.1</ip>
    <port>92</port>
</item>
```

```cpp
bool Init(const pugi::xml_node& xml_node) {
    pugi::xml_node item_node = xml_node.child("item");
    while(item_node) {
        _item_ item;
        item.Init(item_node);
        this->item_.push_back(item_node);
        item_node = item_node.next_sibling("item");
    }
    return true;
}

struct _item_ {
    pugi::xml_attribute id;

    bool Init(const pugi::xml_node& xml_node) {
        this->id = xml_node.attribute("id");
        pugi::xml_node ip_node = xml_node.child("ip");
        if (!ip_node) {
            return false;
        }
        this->ip_.Init(xml_node);
        return true;
    }

    struct _ip_ {
        pugi::xml_text text_;
        bool Init(const pugi::xml_node& xml_node) {
            this->text_ = xml_node.text();
            return true;
        }
    };
    _ip_ ip_;
    pugi::xml_text ip() const { return this->ip_.text_; }
};
std::vector<_item_> item_;

const _item_& item() const { return this->item_; }
```

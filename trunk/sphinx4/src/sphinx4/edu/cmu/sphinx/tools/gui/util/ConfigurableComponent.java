/*
 * ConfigurableComponent.java
 *
 * Created on November 29, 2006, 4:40 PM
 *
 * Portions Copyright 2007 Mitsubishi Electric Research Laboratories.
 * Portions Copyright 2007 Harvard Extension Schoool, Harvard University
 * All Rights Reserved.  Use is subject to license terms.
 * 
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL 
 * WARRANTIES.
 */

package edu.cmu.sphinx.tools.gui.util;

import java.util.HashMap;
import java.util.Map;
import java.util.Iterator;
import java.util.List;

import edu.cmu.sphinx.tools.gui.RawPropertyData;

/**
 * This class holds information about one configurable class in the Sphinx model
 * Which includes the classname, property list, class description,group name,  
 * and the configuration sets that are defined for this class
 * <p>
 * This class, combined with <code>ConfigurableProperty</code> construct
 * the Sphinx model. 
 * <p>
 * RawPropertyData is used to hold the configuration values set for this component
 *
 * @author Ariani
 * @see ConfigurableProperty
 * @see RawPropertyData
 */
public class ConfigurableComponent {
    
    private String _sectionName;
    private Map _propList; // String propertyName, ConfigurableProperty 
    private Class _component;    
    private String _componentDesc;
    private String _componentClassName;
    private Map _confProp; // List of String name, Rpd for this class
    
    /** 
     * Creates a new instance of <code>ConfigurableComponent</code>
     * 
     * @param section the group that this class belongs to
     * @param component reference to the class that this object represents
     * @param name class name
     * @param desc brief description of this configurable component
     */
    public ConfigurableComponent(String section, Class component, String name, String desc) {
        _sectionName = section;
        _component = component;
        _componentClassName = name;
        _componentDesc = desc;
       _propList = new HashMap();
       _confProp = new HashMap();
    }
        
    /**
     * used to list the information contained in this component
     * mainly for debugging and testing
     *
     * @return string description of model
     */
    public String toString(){
        String output = "Section :" + _sectionName + '\n';
        output = output.concat(" Class Name : " + _componentClassName+'\n');
        if (!_componentDesc.isEmpty()) {
            output = output.concat(" Desc : "+ _componentDesc+'\n');
        }
        if( !_propList.isEmpty() ){
            output = output.concat(" Property list : \n");
            for (Iterator it = _propList.values().iterator();it.hasNext();){
                output = output.concat(((ConfigurableProperty)it.next()).toString());
            }
        }        
        if ( !_confProp.isEmpty()){
            output = output.concat(" Configuration values : ***** \n");
            for (Iterator it = _confProp.values().iterator();it.hasNext();){
                output = output.concat(((RawPropertyData)it.next()).toString());
            }
        }
        return output;
    }
       
    /**
     * @return Returns group / section name that this component belongs to
     */
    public String getSectionName(){
        return _sectionName;
    }
    
    /**
     * @return Returns the Map that holds the component properties 
     *         (pair of <code>String, ConfigurableProperty</code>)
     */
    public Map getPropertyMap(){
        return _propList;
    }
    
    /**
     * Add a new property for this component
     *
     * @param cp <code>ConfigurableProperty</code>
     */
    public void addProperty(ConfigurableProperty cp){
        _propList.put(cp.getName(),cp);
    }
    
    /**
     * Check if component has the property
     *
     * @param name Name of property 
     * @return Boolean <code>true</code> if property exists
     */
    public boolean containsProperty(String name){
        return _propList.containsKey(name);
    }
    
    /** 
     * get the property with specified name
     *
     * @param name Name of property
     * @return <code>ConfigurableProperty</code> with the specified name;
     *         returns null if the property does not exist
     */
    public ConfigurableProperty getProperty(String name){
        ConfigurableProperty cp = (ConfigurableProperty)_propList.get(name);
        if (cp == null){
            return null;
        }
        else return cp;        
    }
    
    /**
     * @return the <code>Class</code> that this component refers to
     */
    public Class getComponentClass(){
        return _component;
    }
    
    /**
     * @return class name of component
     */
    public String getName(){
        return _componentClassName;
    }
       
    /**
     * @return class description
     */
    public String getDesc(){
        return _componentDesc;
    }
    
    /**
     * This function is used to attach a configuration set for this component           
     * 
     * @param rpd <code>RawPropertyData</code> that holds the property values
     */
    public void addConfigurationProp(RawPropertyData rpd){
        _confProp.put(rpd.getName(),rpd);        
    }
        
    /**
     * Delete all configuration sets from this component
     */
    public void deleteAllConfigurationProp(){
        _confProp.clear();
    }
    
    /**
     * Delete the configuration set with specified name
     *
     * @param rpdname name of <code>RawPropertyData</code> to be deleted
     */
    public void deleteConfigurationProp(String rpdname){
        if ( _confProp.containsKey(rpdname)) {
            _confProp.remove(rpdname);
        }
    }
    

    
    /** 
     * Remove a specific property from one of the configuration
     * sets
     *
     * @param rpdname Name of <code>RawPropertyData</code> that stores the
     *               property values
     * @param propname Property name to be modified    
     */
    public void deleteOneConfigurationPropFromSet(String rpdname,String propname){
        RawPropertyData rpd = (RawPropertyData)_confProp.get(rpdname);
        rpd.remove(propname);
    }
    
    /**   
     * Change the value of a specific property inside one of the configuration
     * sets
     *
     * @param rpdname Name of <code>RawPropertyData</code> that stores the
     *               property values
     * @param propname Property name to be modified
     * @param newvalue New value of the property
     */
    public void changeConfigurationPropValue(String rpdname,String propname,
            String newvalue){
        RawPropertyData rpd = (RawPropertyData)_confProp.get(rpdname);
        rpd.change(propname,newvalue);
    }
    
    /** 
     * Change the value of a specific property inside one of the configuration
     * sets
     *
     * @param rpdname Name of <code>RawPropertyData</code> that stores the
     *               property values
     * @param propname Property name to be modified
     * @param newvalue List of new values of the property
     */
    public void changeConfigurationPropValue(String rpdname,String propname,
            List newvalue){
        RawPropertyData rpd = (RawPropertyData)_confProp.get(rpdname);
        rpd.change(propname,newvalue);
    }
        
    /**
     * Get current value of the configuration
     *
     * @param rpdName Name of <code>RawPropertyData</code> that holds the 
     *              configuration values
     * @param propname Property name
     * @return Either a <code>String</code> or <code>List</code> that 
     *          contains value of the property
     */
    public Object getConfigurationPropValue(String rpdName,String propname){        
        if( _confProp.containsKey(rpdName) ){
            RawPropertyData rpd = (RawPropertyData)_confProp.get(rpdName);
            if ( rpd.getProperties().containsKey(propname)) {
                return rpd.getProperties().get(propname);
            }
            else
                return null;
        }
        else
            return null;
    }
    
    /**
     * @return <code>Map</code> of <code>RawPropertyData</code> that has all the
     *          configuration sets of this component. Each entry consists of 
     *          String setname, RawPropertyData setproperties
     */
    public Map getConfigurationPropMap(){
        return _confProp;
    }
    
    /**
     * check if this component has a <code>RawPropertyData</code>
     * configuration set with specified name
     *
     * @return <code>true</code> if the configuration with specified name exists
     */
    public boolean containsConfigurationSet(String name){
        return _confProp.containsKey(name);
    }

    
    /**
     * add all the properties that are in the Configurable Component model,
     * by adding them as default
     */
    private void addDefaultProps(RawPropertyData rpd){
        Map completePropMap = this.getPropertyMap();
        for (Iterator it = completePropMap.entrySet().iterator();it.hasNext();){
            Map.Entry propentry = (Map.Entry)it.next();
            String propname = (String)propentry.getKey();
            // System.out.println("***** prop"+propname);           
            
            // if it doesn't exist yet
            if( !rpd.contains(propname) ){
                ConfigurableProperty prop = (ConfigurableProperty)propentry.getValue();
                String defaultVal = prop.getDefault();
                if (defaultVal != null && !defaultVal.trim().isEmpty()) {
                    rpd.add(propname,defaultVal);
                    // System.out.println("***** add prop "+propname);
                }
                else {
                    rpd.add(propname, (String)null);           
                    System.out.println("***** add null prop "+propname);
                }
            }
        }
    }

    /** 
     * This function is used to create a new configuration set for this type of class
     * The property values are set as their default values
     * 
     * @param setname Name of new configuration set
     */
    public void createNewSet(String setname){
        // System.out.println("*****" + this);
        // the set name given should have been checked by GUI
        RawPropertyData newrpd = new RawPropertyData(setname,_componentClassName);        
        addDefaultProps(newrpd); // fill them up with default properties
        this.addConfigurationProp(newrpd); //add new rpd to this ConfigurableComponent's set        
    }
    
}

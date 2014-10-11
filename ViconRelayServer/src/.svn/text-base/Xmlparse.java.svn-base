import javax.xml.*;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.w3c.dom.*;
import java.io.*;

public class Xmlparse  {
	public static void main(String argv[]) {
		try{
			File fXML = new File(argv[0]);
			DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
			DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();
			Document doc = dBuilder.parse(fXML);
			
			doc.getDocumentElement().normalize();
			
			
			printNote(doc.getChildNodes(), 0);
			/*
			System.out.println("Root element :" + doc.getDocumentElement().getNodeName());
			
			//Get the nodelist of objects
			NodeList nList = doc.getElementsByTagName("object");
			
			System.out.println("-----");
			
			for (int temp = 0; temp < nList.getLength(); temp++) {
				Node nNode = nList.item(temp);
				
				System.out.println("\nCurrent Element :"+ nNode.getNodeName());
				
				if (nNode.getNodeType() == Node.ELEMENT_NODE) {
					
					Element eElement = (Element) nNode;
					Node tempNode = null;
					
					NodeList nListChildren = nNode.getChildNodes();
					
					for(int j = 0; j < nListChildren.getLength(); j++ ){
						Node cNode = nListChildren.item(j);
						System.out.println("\tElement " + cNode.getNodeName());
						
					}
					
					
					//System.out.println(eElement.getAttributes().toString() + "\n");
					
				}//end if 
				
				
			}//end for each field
			*/
		} catch (Exception e){
			e.printStackTrace();
		}//end try/catch
	}//end static main
	
	public static void travTree(Node n, int depth){
		String print = "";
		for (int i = 0; i < depth; i++){
			print +="\t";
		}
		//print+= n.getNodeName();
		if (n.getNodeType() == Node.ELEMENT_NODE)
		if (n.getNodeType() == Node.TEXT_NODE){
			print += n.getNodeValue();
			System.out.println(print);
			
		}
		
		if (n.hasChildNodes()){
			travTree(n.getFirstChild(), depth+1);
		}
		
		if (n.getNextSibling() != null){
			travTree(n.getNextSibling(), depth);
		}
		
		
	}//end travtree
	
	 private static void printNote(NodeList nodeList, int depth) {
		 String tabs = "";
		 for(int i = 0; i < depth; i++){
			 tabs += "  ";
		 }//Set up tabs
		 
		    for (int count = 0; count < nodeList.getLength(); count++) 
		    {
		 
				Node tempNode = nodeList.item(count);
				
				if (tempNode.getNodeType() == Node.TEXT_NODE){
					System.out.print("\t"+tempNode.getTextContent()+"\t");
					
				}
				
				// make sure it's element node.
				if (tempNode.getNodeType() == Node.ELEMENT_NODE) {
			 
					// get node name and value
				
					System.out.print("Children: "+tempNode.getChildNodes().getLength());
					System.out.println("\tAttributes: "+(tempNode.getAttributes().getLength()));
					System.out.print(tabs+"Node Name =" + tempNode.getNodeName() + " [OPEN]");
					//System.out.print("-- Node Value =" + tempNode.getTextContent()+"\n");
			 
					if (tempNode.hasAttributes()) {
			 
						// get attributes names and values
						NamedNodeMap nodeMap = tempNode.getAttributes();
			 
						for (int i = 0; i < nodeMap.getLength(); i++) {
			 
							Node node = nodeMap.item(i);
							
							System.out.print(tabs+"<" + node.getNodeName()+"> = ");
							System.out.print(tabs+"( "+ node.getNodeValue()+ " )\n");
			 
						}//end for each attribute
			 
					}//end if has attributes
					
					
			 
					if (tempNode.hasChildNodes()) {
			 
						// loop again if has child nodes
						printNote(tempNode.getChildNodes(), depth+1);
			 
					}//end if has children
			 
					System.out.print("</Node Name = " + tempNode.getNodeName() + " [CLOSE]>\n");
			 
				}//end if is element node
			 
				
		    }//end for each node
		 
	 }
		 
	
}//end class

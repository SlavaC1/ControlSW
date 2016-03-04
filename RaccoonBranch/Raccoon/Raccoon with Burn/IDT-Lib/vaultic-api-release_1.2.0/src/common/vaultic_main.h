/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */

/**
 * \mainpage Vault IC Application Programmers' Interface (API)
 * 
 * The Vault IC product family offers a comprehensive security solution to a 
 * wide variety of secure applications.  It features a set of standard public 
 * domain cryptographic algorithms, as well as other supporting services. 
 *
 * The VaultIC API provides an easy and quick way of accessing those services 
 * through a remote method invocation mechanism. The primary function of the 
 * Vault IC API is to serialise and de-serialise the remote invocation method 
 * data to and from the VaultIC respectively. 
 * 
 * The main user interface is outlined in \ref VaultIcApi, henceforth known as
 * the API. The API methods, where possible, have a 1 to 1 correspondence with 
 * the commands supported by the VaultIC. The API method signatures reflect the 
 * parameters passed to those commands as specified in the VaultIC Generic 
 * Datasheet.
 *
 * Where possible certain parameter values can be substituted by a set of 
 * predefined constants declared as macros, a listing of those can be found in 
 * \ref Constants. It is strongly advised to make full and consistent use of  
 * those constants as those will ensure your application is immune to future 
 * API release changes.
 * 
 * All the API methods return a status code, upon successful completion the 
 * status code will be set to #VLT_OK. The only way the API can signal an 
 * erroneous conditions is through the return value. The return value, however, 
 * provides you with a simple mechanism to determine the origin of the erroneous 
 * condition. A status code with a value larger than #VLT_OK implies that the
 * erroneous condition occured within the API code, otherwise a status code with 
 * a value less than VLT_OK implies that the erroneous condition is an APDU status
 * word returned by the VaultIC.
 * 
 * This manual is partitioned into the following sections:
 * - \ref VaultIcApi
 * - \ref VaultIcLib
 * - \ref Constants 
 * - \ref Structures
 * - \ref VaultIcKeyWrapping
 * - \ref VaultIcIdentityAuthentication
 * - \ref VaultIcFileSystem
 * 
 * \section Copyright
 *
 * © Inside Contactless 2010. All Rights Reserved. Inside Contactless®, Inside 
 * Contactless logo and combinations thereof, and others are registered trademarks 
 * or tradenames of Inside Contactless or its subsidiaries. Other terms and product 
 * names may be trademarks of others.
 *
 *
 * \section Disclaimer
 *
 * All products are sold subject to Inside Contactless Terms & Conditions of 
 * Sale and the provisions of any agreements made between Inside Contactless 
 * and the Customer. In ordering a product covered by this document the 
 * Customer agrees to be bound by those Terms & Conditions and agreements and 
 * nothing contained in this document constitutes or forms part of a contract
 * (with the exception of the contents of this Notice). A copy of Inside 
 * Contactless’ Terms & Conditions of Sale is available on request. Export of 
 * any Inside Contactless product outside of the EU may require an export 
 * Licence.The information in this document is provided in connection with 
 * Inside Contactless products. No license, express or implied, by estoppel or 
 * otherwise, to any intellectual property right is granted by this document or 
 * in connection with the sale of Inside Contactless  products. EXCEPT AS SET 
 * FORTH IN INSIDE CONTACTLESS’ TERMS AND CONDITIONS OF SALE,  INSIDE CONTACTLESS 
 * OR ITS SUPPLIERS OR LICENSORS ASSUME NO LIABILITY WHATSOEVER AND DISCLAIMS 
 * ANY EXPRESS, IMPLIED OR STATUTORY WARRANTY RELATING TO ITS PRODUCTS INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTY OF MERCHANTABILITY, SATISFACTORY 
 * QUALITY, FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT. IN NO EVENT 
 * SHALL  INSIDE CONTACTLESS BE LIABLE FOR ANY DIRECT, INDIRECT, CONSEQUENTIAL, 
 * PUNITIVE, EXEMPLARY, SPECIAL OR INCIDENTAL DAMAGES (INCLUDING, WITHOUT 
 * LIMITATION, DAMAGES FOR LOSS OF PROFITS, LOSS OF REVENUE, BUSINESS 
 * INTERRUPTION, LOSS OF GOODWILL, OR LOSS OF INFORMATION OR DATA) NOTWITHSTANDING 
 * THE THEORY OF LIABILITY UNDER WHICH SAID DAMAGES ARE SOUGHT, INCLUDING BUT NOT 
 * LIMITED TO CONTRACT, TORT (INCLUDING NEGLIGENCE), PRODUCTS LIABILITY, STRICT 
 * LIABILITY, STATUTORY LIABILITY OR OTHERWISE, EVEN IF INSIDE CONTACTLESS HAS 
 * BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES. Inside Contactless makes no 
 * representations or warranties with respect to the accuracy or completeness of 
 * the contents of this document and reserves the right to make changes to 
 * specifications and product descriptions at any time without notice.  Inside 
 * Contactless does not make any commitment to update the information contained 
 * herein. Inside Contactless advises its customers to obtain the latest version 
 * of device data sheets to verify, before placing orders, that the information 
 * being relied upon by the customer is current. Inside Contactless products are 
 * not intended, authorized, or warranted for use as critical components in life 
 * support devices, systems or applications, unless a specific written agreement 
 * pertaining to such intended use is executed between the manufacturer and Inside 
 * Contactless. Life support devices, systems or applications are devices, systems 
 * or applications that (a) are intended for surgical implant to the body or (b) 
 * support or sustain life, and which defect or failure to perform can be reasonably 
 * expected to result in an injury to the user.
 * A critical component is any component of a life support device, system or 
 * application which failure to perform can be reasonably expected to cause the 
 * failure of the life support device, system or application, or to affect its 
 * safety or effectiveness. The security of any system in which the product is 
 * used will depend on the system’s security as a whole. Where security
 * or cryptography features are mentioned in this document this refers to features 
 * which are intended to increase the security of the product under normal use and 
 * in normal circumstances.
 *
 */
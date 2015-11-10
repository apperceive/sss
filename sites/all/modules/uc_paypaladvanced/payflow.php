<?php
/*payflow.php * *used to store functions for the payflow class * */ 
class payflow
{ 
	/*__construct()   *   *Creates the Payflow Object and stores the   *credentials for the API caller   */ 

	public function __construct()
	{     
		$this->user = variable_get('uc_paypaladvanced_user', '');     
		$this->vendor = variable_get('uc_paypaladvanced_vendor', '');
		$this->partner = variable_get('uc_paypaladvanced_partner', '');     
		$this->pwd = variable_get('uc_paypaladvanced_password', '');     
		$this->secure_token_url = (variable_get('uc_paypaladvanced_mode', '')=='test')?'https://pilot-payflowpro.paypal.com':'https://payflowpro.paypal.com';
		$this->mode =  variable_get('uc_paypaladvanced_mode', '');     
	}
	//end constructor
	
	/*getSecureToken()
	 *   *This function sets the values for the   *secureTokenID, invoice number, and assigns   
	*other variables as needed for the call   *this uses hard coded values for example   *purposes.   *   *@return array $response   */ 
	public function getSecureToken($order)
	{
		 $country = uc_get_country_data(array('country_id' => $order->billing_country));
		if ($country === FALSE) {
		  $country = array(0 => array('country_iso_code_3' => 'USA'));
		}
		//$countr = uc_get_state_data(array('state_id' => $order->billing_state));
		//create a SECURETOKENID   
		$secureTokenID = $this->guid();   
		
		//create a unique INVNUM   
		$invnum = "INV" . $this->guid();
		
		$reqstr ="TRXTYPE=".variable_get('uc_paypaladvanced_transaction', '');
		$reqstr.="&TENDER=C";
		$reqstr.="&INVNUM=". $invnum;
		$reqstr.="&AMT=".uc_currency_format($order->order_total, FALSE, FALSE, '.');
		$reqstr.="&CURRENCY=".$order->currency;
		$reqstr.="&PONUM=".$order->order_id;
		
		
		$reqstr.="&BILLTOFIRSTNAME=".$order->billing_first_name;
		$reqstr.="&BILLTOLASTNAME=".$order->billing_last_name;
		$reqstr.="&BILLTOEMAIL=".$order->primary_email;
		$reqstr.="&BILLTOPHONE=".$order->billing_phone;
		$reqstr.="&BILLTOSTREET=".$order->billing_street1 . ', ' . $order->billing_street2;
		$reqstr.="&BILLTOCITY=".$order->billing_city;
		$reqstr.="&CURRENCYCODE=".$order->currency;
		$reqstr.="&BILLTOSTATE=".$order->billing_zone;
		$reqstr.="&BILLTOZIP=".$order->billing_postal_code;
		$reqstr.="&BILLTOCOUNTRY=".$country[0]['country_iso_code_3'];
		
		$reqstr.="&CREATESECURETOKEN=Y";
		$reqstr.="&SECURETOKENID=". $secureTokenID;
		$reqstr.="&VERBOSITY=HIGH";
		
		$credstr = "&USER=" . $this->user . "&VENDOR=" . $this->vendor . "&PARTNER=" . $this->partner . "&PWD=" . $this->pwd;
		
		//combine the strings   
		 $nvp_req = $reqstr. $credstr;
		 
		//set the endpoint to a local var:
		 
		 $endpoint = $this->secure_token_url;   //make the call   
		 $res = $this->PPHttpPost($endpoint, $nvp_req);   
		 return $res; 
	}
	 //end getToken call /*PPHttpPost(string, string)   *   *PPHttpPost takes in two strings, and   *makes a curl request and returns the result.   *   *@return array $httpResponseAr   */ 
	 public function PPHttpPost($endpoint, $nvpstr)
	 {
		// setting the curl parameters. 
		$ch = curl_init(); 
		curl_setopt($ch, CURLOPT_URL, $endpoint); 
		curl_setopt($ch, CURLOPT_VERBOSE, 1);
		// turning off the server and peer verification(TrustManager Concept). 
		curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, FALSE); 
		curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, FALSE);

		curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1); 
		curl_setopt($ch, CURLOPT_POST, 1);

// setting the NVP $my_api_str as POST FIELD to curl 
		curl_setopt($ch, CURLOPT_POSTFIELDS, $nvpstr);

// getting response from server 
		$httpResponse = curl_exec($ch);

		if(!$httpResponse) 
		{    
			$response = "$API_method failed: ".curl_error($ch).'('.curl_errno($ch).')';    
			return $response; 
		} 
		$httpResponseAr = explode("&", $httpResponse);
		$httpResponseAr;
		return $httpResponseAr;

	}
//end PPHttpPost function 
/*getMySecureToken(array)   *   *This takes in the response array from the   *PPHttpPost call, and parses out the SecureToken   *   *This is used because the securetoken may contain   *an "=" sign   *   *@return string $secure_token   */ 
	public function getMySecureToken($response)
	{   
		$secure_token = $response[1];   
		$secure_token = substr($secure_token, -25);   
		return $secure_token; 
	}
	//end getSecureToken() /*parseResponse(array)   *   *This function parses out the response without taking   *into account that the securetoken may contain an "="   *sign. The only thing you need from this is the   *SecureTokenID.   *   *@return array $parsed_response   */ 
	public function parseResponse($response)
	{
		$parsed_response = array(); 
		foreach ($response as $i => $value) 
		{   
			$tmpAr = explode("=", $value);   
			if(sizeof($tmpAr) > 1) 
			{    
				$parsed_response[$tmpAr[0]] = $tmpAr[1];   
			} 
		} 
		return $parsed_response; 
	}//end parseResponse
	/*guid()   *   *This function creates an MD5 hash of a timestamp   *to be used with the SecureTokenID, and Invnum   *in the initial call   *   *@return string $str   */ 
	public function guid()
	{ 
		//hash out a timestamp and some chars to get a 25 char token to pass in 
		$str = date('l jS \of F Y h:i:s A'); 
		$str = trim($str); 
		$str = md5($str);
		return $str; 
	}//end guid 
}//end class

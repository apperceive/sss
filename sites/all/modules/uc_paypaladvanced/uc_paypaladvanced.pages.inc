<?php

/**
 * @file
 * paypaladvanced menu items.
 */

/**
 * Finalizes paypaladvanced transaction.
 */
function uc_paypaladvanced_complete($cart_id=0) {
  
  // note needed (igonored):
  // session_start();
  
  global $user;
  // If the order ID specified in the return URL is not the same as the one in
  // the user's session, we need to assume this is either a spoof or that the
  // user tried to adjust the order on this side while at PayPal. If it was a
  // legitimate checkout, the IPN will still come in from PayPal so the order
  // gets processed correctly. We'll leave an ambiguous message just in case.
  /*
Array ( [AVSZIP] => X [STATE] => 1 [TYPE] => A [ACCT] => 4444 [PROCCVV2] => M [PPREF] => R378S45DZUTQPIY0R [PONUM] => 40 [CVV2MATCH] => Y [LASTNAME] => test [PNREF] => E79P3A8EFB74 [TENDER] => CC [EMAIL] => demo@egrovetech.com [METHOD] => CC [SHIPTOCOUNTRY] => US [AMT] => 499.00 [CORRELATIONID] => 2c9706997458s [TRANSTIME] => 2012-10-10 22:29:37 [INVOICE] => INVd4584d079b24d425d [ZIP] => 65432 [AUTHCODE] => 111111 [EXPDATE] => 0119 [IAVS] => N [RESPMSG] => Approved [COUNTRY] => USA [BILLTOZIP] => 65432 [BILLTOCITY] => test [TAX] => 0.00 [BILLTOSTATE] => 1 [CARDTYPE] => 1 [FIRSTNAME] => test [AVSDATA] => XXN [PROCAVS] => U [SECURETOKEN] => 19ZwL9GGcPUCcEwEFx7ru0giz [SECURETOKENID] => d4584d079b24d425df7d40d8aba2427e [INVNUM] => INVd4584d079b24d425d [AVSADDR] => X [CITY] => test [NAME] => test test [BILLTOSTREET] => test, test [COUNTRYTOSHIP] => US [RESULT] => 0 [ADDRESS] => test, test )
  */
  $transid = $_REQUEST['PNREF']; 
  $result = $_REQUEST['RESULT']; 
  //$last4 = $_REQUEST['ACCT']; 
  //$amount = $_REQUEST['AMT']; 
  //$cardType = $_REQUEST['CARDTYPE']; 
  $respMsg = $_REQUEST['RESPMSG']; 
  //$invnum = $_REQUEST['INVNUM'];
  $transtype = $_REQUEST['TYPE'];
  $ponum = $_REQUEST['PONUM'];
  $e=0;
  
  watchdog('mss', "variables: " . var_export(get_defined_vars(), 1));

  
  $order = uc_order_load($ponum);       // MSS: why ponum? 
  // consider: 
  // index is not defined, set it to $ponum?
  // $order = uc_order_load(intval($_SESSION['cart_order']));
  
  /*
  if ($order === FALSE || uc_order_status_data($order->order_status, 'state') != 'in_checkout') {
    drupal_set_message(t("An error has occurred during payment.  Please contact us to ensure your order has submitted. Reference 1<br/>
                         Message : ".$respMsg),'error');
    $url='cart';
    $e=1;
  }

  // TODO: should this be uc_cart_id?
  
  if (!isset($_SESSION['cart_order']) || intval($_SESSION['cart_order']) !=$ponum) {
    drupal_set_message(t("An error has occurred during payment.  Please contact us to ensure your order has submitted. Reference 2<br/>
                         Message : ".$respMsg),'error');
    $url='cart';
    $e=1;
  }
  */
  
  if($e!=1)
  {
    if($result == "0")
    {
        // second arg copied from cart/checkout/complete
        // uc_cart_complete_sale($order);
        $build = uc_cart_complete_sale($order, variable_get('uc_new_customer_login', FALSE));
        uc_order_update_status($order->order_id, 'payment_received');
        
        db_insert('uc_payment_paypal_advanced')
          ->fields(array(
            'order_id' => $ponum,
            'transaction_id' => $transid,
            'transaction_type' => $transtype,
          ))
          ->execute();          
          
        // copied from cart/checkout/complete, slightly modified
        // Add a comment to let sales team know this came in through the site.
        uc_order_comment_save($order->order_id, 0, t('Order created through website via PayPal.'), 'admin');
        
        // CONSIDER: Do we want to empty the cart before going to cart/checkout/complete?
        // NOTE: cart_id is typically 0 for anon
        uc_cart_empty($cart_id);
        //uc_cart_empty();
          
        // copied from cart/checkout/complete
        // unset($_SESSION['uc_checkout'][$order->order_id], $_SESSION['cart_order']);
        
        // copied from cart/checkout/complete
        // This lets us know it's a legitimate access of the complete page.
        $_SESSION['uc_checkout'][$_SESSION['cart_order']]['do_complete'] = TRUE;
        
        
        
        $url='cart/checkout/complete';
    }
    else
    {
      drupal_set_message(t("An error has occurred during payment.<br/>
                           Message : ".$respMsg),'error');
      $url='cart';
    }
  }
 

/*
  dpm(get_defined_vars());
  dpm($_SESSION);
  dpm($_REQUEST);
*/

  //watchdog('mss', "uc_paypaladvanced_complete, cart_id: $cart_id, url: $url, error: $e, REQUEST: " . var_export($_REQUEST, 1) . ", ORDER:  " . (is_object($order) ? $order->order_status : $order));

  watchdog('mss', "uc_paypaladvanced_complete, SESSION: " . var_export($_SESSION, 1));

 
  // todo: consider returning $build as cart/checkout/complete does
  print drupal_render($build);


  // TODO: set $_SESSION['cart_order'] before calling cart/checkout/complete
  //$_SESSION['cart_order'] = $ponum;


  // how make two links use target='top'
  
  
/*
?>
	<script type="text/javascript">
        window.top.location = "<?php echo url($url, array('absolute' => TRUE));?>";
	</script>
<?php
*/

}
?>
